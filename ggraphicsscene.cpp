#include <QColor>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsView>
#include <QList>
#include <QSet>

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

#include "ggraphicsscene.h"
#include "gcommitarrow.h"
#include "localrepoparser.h"
#include "logger.h"
#include "mainwindow.h"
#include "gbranchlabel.h"
#include "glabelline.h"
#include "gcommitnode.h"
#include "commitnode.h"
#include "branch.h"

GGraphicsScene::GGraphicsScene(QObject *parent) : QGraphicsScene(parent) {


}


GCommitNode *GGraphicsScene::convertCommitNodeToGCommitNode(CommitNode* commitNode, GCommitNode* parent, int nodeDepth) {

    // Check if this gcommit node has already been instantiated
    GCommitNode *gCommitNode;
    bool reusingExistingNode = false;
    if(this->allGCommitNodes->find(commitNode->getSha1().getFullString()) != allGCommitNodes->end()) {
        gCommitNode = this->allGCommitNodes->at(commitNode->getSha1().getFullString());
        reusingExistingNode = true;

    }
    // Otherwise make a new one
    else {
        gCommitNode = new GCommitNode();

        // Set attributes for this g node
        gCommitNode->setSha(commitNode->getSha1());
        gCommitNode->setCommitter(commitNode->getCommitter());
        gCommitNode->setAuthor(commitNode->getAuthor());
        gCommitNode->setDateAndTime(commitNode->getCommitTime());
        gCommitNode->setMessage(commitNode->getMessage().toStdString());
    }

    // If this is a recursive call, we'll have a parent to attach
    if (parent != 0) {
        gCommitNode->getParentGNodes()->insert(parent);
    }

    // Node depth should be maximum of all depths calculated for this node (which differ
    // based on which parent we came from
    if (reusingExistingNode) {
        gCommitNode->setDepth(max(nodeDepth, gCommitNode->getDepth()));
    }
    // New node, no max needed
    else {
        gCommitNode->setDepth(nodeDepth);
    }

    // If there are any children, recursively call this on them
    QSet<CommitNode *>* children = commitNode->getChildren();
    if (children->size() > 0) {
        // Increase the node depth for these children
        nodeDepth++;
        // Recursively call ourselves for each child and add result to our set of children
        for (QSet<CommitNode *>::iterator it = children->begin(); it !=children->end(); ++it ) {
            GCommitNode * newNode = convertCommitNodeToGCommitNode(*it, gCommitNode, nodeDepth);
            gCommitNode->getChildrenGNodes()->insert(newNode);
            // Add an arrow from this to the child to the global set of arrows
            GCommitArrow * newArrow = new GCommitArrow(gCommitNode, newNode);
            this->arrows->push_back(newArrow);
        }
    }

    // Add this commit to a list of all commits, if it hasn't already been
    if (!reusingExistingNode) {
        this->allGCommitNodes->insert({commitNode->getSha1().getFullString(), gCommitNode});
    }
    return gCommitNode;
}

int GGraphicsScene::measurePhase(GCommitNode *node) {

    // Retrieve the children of ours for whom we are the youngest parent
    set<GCommitNode *> *children = node->getCloseChildren();

    // Recurse down tree if we are an inner node
    if (children->size() > 0) {
        int leavesOnSubTrees = 0;
        for (set<GCommitNode *>::iterator it = children->begin(); it !=children->end(); ++it ) {
            leavesOnSubTrees += measurePhase(*it);
        }
        // Number of leaves to a node is the leaves on all of its subtrees added together
        node->setNumberOfLeaves(leavesOnSubTrees);
        return leavesOnSubTrees;
    }
    // Otherwise we are a leaf
    else {
        // We have no leaves
        node->setNumberOfLeaves(0);
        // Our parent gets +1 leaves
        return 1;
    }
}

void GGraphicsScene::renderPhase(GCommitNode *node) {

    // Figure out allocated width for whole tree
    int totalAllocatedWidth = node->getNumberOfLeaves() * CANVAS_SPACE_PER_NODE;

    // Render tree starting at root node
    this->renderNode(node, 0, totalAllocatedWidth);

    // Render arrows
    for (vector<GCommitArrow *>::iterator arrow = this->arrows->begin(); arrow != this->arrows->end(); arrow++) {
        this->addItem(*arrow);
    }

    // Refresh
    this->update();

}



void GGraphicsScene::renderNode(GCommitNode *node, int startX, int endX) {

    // Position yourself in the middle of your allocated width
    int xPos = startX + ((endX - startX) / 2.0);
    node->setPos(xPos, CANVAS_ROW_OFFSET + CANVAS_ROW_HEIGHT * node->getDepth());

    // Render
    this->addItem(node);

    // If we're a leaf, we're done
    if (node->getChildrenGNodes()->empty()) {
        return;
    }

    // Divide up your allocated space into even slots for each child (later to be adjusted based on
    // subtree leaf number)
    int spacePerChild = ((int)((double)(endX - startX) / (double)node->getCloseChildren()->size()));

    // Iterate over children, allocate them space inside us, and render them
    int childNumber = 0;

    set<GCommitNode *> *children = node->getCloseChildren();
    for (set<GCommitNode *>::iterator it = children->begin(); it !=children->end(); ++it ) {
        (*it)->setChildRanking(childNumber);
        this->renderNode(*it, (int)((startX + (spacePerChild * childNumber))), (int)(startX + ((spacePerChild * (childNumber + 1)))));
        childNumber++;
    }
}

void GGraphicsScene::renderRepository(string repoPath) {

    this->clear();

    this->currentRepoPath = repoPath;

    this->allGCommitNodes = new map<string, GCommitNode *>();
    this->arrows = new vector<GCommitArrow *>();

    CommitNode *rootCommit = LocalRepoParser::getGitTree(repoPath, *this);

    // Ensure we recieve a repo back from the parser
    if (rootCommit == 0) {
       Logger::error("GGraphicsScene", "Nothing returned from parser. Invalid git repository path? " + repoPath + " Aborting.");
       return;
    }

    GCommitNode *root = convertCommitNodeToGCommitNode(rootCommit);
    // Ensure we recieve a gcommitnode back
    if (root == 0) {
       Logger::error("GGraphicsScene", "Nothing returned from convertCommitNodeToGCommitNode. Aborting. ");
       return;
    }

    Logger::info("GGraphicsScene", "Parsed and converted repo at path " + repoPath);

    // Retrieve the list of branches
    QList<Branch *> branches = LocalRepoParser::getBranches(repoPath);

    // Ensure that we get at least 1 branch
    if (branches.empty()) {
        Logger::error("GGraphicsScene", "No branches recieved from parser. Aborting");
        return;
    }

    // Measure tree
    int totalLeaves = this->measurePhase(root);

    Logger::info("GGraphicsScene", "Finished measure phase");
    // Size canvas coordinate grid based on measurement
    this->setSceneRect(0, 0, totalLeaves * CANVAS_SPACE_PER_NODE, 1000); //TODO fix number
    this->setBackgroundBrush(QBrush(CANVAS_BG_COLOR, Qt::SolidPattern));

    // Render tree
    this->renderPhase(root);
    Logger::info("GGraphicsScene", "Finished render phase");

    // Render branches
    this->renderBranchLabels(branches);

}

string GGraphicsScene::getCurrentRepoPath() const { return currentRepoPath; }

void GGraphicsScene::setCurrentRepoPath(const string &value) { currentRepoPath = value; }

void GGraphicsScene::refreshRepo() {
    this->clear();
    this->renderRepository(this->currentRepoPath);
}

void GGraphicsScene::notifyRepoChange() {

    this->refreshRepo();

}

void GGraphicsScene::renderBranchLabels(QList<Branch *> branches) {

    //Iterate over branches
    QListIterator<Branch *> iter(branches);
    while (iter.hasNext()) {
        // Convert them into GBranchLabels
        Branch * branch = iter.next();
        GBranchLabel *branchLabel = new GBranchLabel(branch);

        // Find commit to which this branch refers
        if(this->allGCommitNodes->find(branch->getCommitSha().getFullString()) != allGCommitNodes->end()) {
            GCommitNode *gCommitNode = this->allGCommitNodes->at(branch->getCommitSha().getFullString());
            // Associate them with each other
            branchLabel->setAssociatedCommit(gCommitNode);
            int branchNum = gCommitNode->addBranchLabel(branchLabel);
            branchLabel->establishPosition(branchNum, gCommitNode->getChildRanking());
            // Add branch label
            this->addItem(branchLabel);
            // Add branch label line
            this->addItem(new GLabelLine(gCommitNode, branchLabel));
        }

    }
}
