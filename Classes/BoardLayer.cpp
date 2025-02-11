//
//  BoardLayer.cpp
//  Dijkstra
//
//  Created by saichi on 2016/02/21.
//
//

#include "BoardLayer.hpp"
#include "NodeLayer.hpp"

USING_NS_CC;
using namespace std;

bool BoardLayer::init(){
    if(!Layer::init()){
        return false;
    }
    winSize = Director::getInstance()->getVisibleSize();
    return true;
}

void BoardLayer::createBoard(int x, int y){
    // ボード(背景)作成
    auto bg = LayerColor::create(Color4B::BLACK, winSize.width, (winSize.width/x)*y);
    this->addChild(bg);
    // ノード(マス)作成
    createNode(x, y);
    setNeighborNode(x, y);
}
void BoardLayer::createNode(int x, int y){
    log("%d * %d マスで生成します", x, y);
    int id = 0;
    for (int i=0; i<x; i++){
        for (int j=0; j<y; j++){
            auto nl = NodeLayer::create();
            nl->setId(id);
            nl->setContentSize(Size((winSize.width/x)-3,(winSize.width/x)-3));
            nl->setKind(random(0, 5));
            nl->setPosition(Vec2((winSize.width/x)*i, (winSize.width/x)*j));
            this->addChild(nl);
            allNode.pushBack(nl);
            id ++;
        }
    }
}
void BoardLayer::setNeighborNode(int x, int y) {
    for (auto node: allNode) {
        int selfId = node->getId();
        // up neighbor
        if ((selfId + 1) % y != 0) {
            auto tNode = allNode.at(selfId + 1);
            node->addNeighborNode(tNode, 1);
        }
        // down neighbor
        if (selfId % y != 0) {
            auto tNode = allNode.at(selfId - 1);
            node->addNeighborNode(tNode, 1);
        }
        // left neighbor
        if (selfId - y >= 0) {
            auto tNode = allNode.at(selfId - y);
            node->addNeighborNode(tNode, 1);
        }
        // right neighbor
        if (selfId + y < x * y) {
            auto tNode = allNode.at(selfId + y);
            node->addNeighborNode(tNode, 1);
        }
    }
}

Vector<NodeLayer*> BoardLayer::searchRoute(int sId, int gId){
    Vector<NodeLayer*> route;
    // ゴールが壁なら即終了
    if (allNode.at(gId)->getKind() == 1) {
        return route;
    }
    breadthFirstSearch(sId, gId);
    // スタートからゴールまでの経路を格納する
    route.pushBack(allNode.at(sId));
    route = setRoute(route, gId);
    if (route.size() > 0) { //成功(壁等なかった)した場合
        route.erase(0);
    }
    return route;
}
/** 幅優先探索 */
void BoardLayer::breadthFirstSearch(int sId, int gId) {
    // 全ノードの探索情報リセット
    for (auto node: allNode) {
        node->dataInit();
    }
    // ゴールノードの探索情報リセット
    allNode.at(gId)->setComeNode(allNode.at(gId));
    allNode.at(gId)->setCost(0);
    allNode.at(gId)->setDepth(0);
    
    Vector<NodeLayer*> searchList;
    searchList.pushBack(allNode.at(gId)); // スタート位置(ゴール)の登録
    while (!searchList.empty()) {
        NodeLayer* searchNode = searchList.at(0);
//        log("%dの隣探索", searchNode->getId());
        // 探索リストの先頭の隣のノード一覧
        int count = 0;
        for (auto neighborNode: searchNode->getNeighborNode()) {
            // 未探索の場合探索リストに追加(depth付ける)
            if (neighborNode->getDepth() == -1) {
                // 壁の場合探索リストに追加しない
                if (neighborNode->getKind() == 1) {
                    //                    continue;
                }else {
                    searchList.pushBack(neighborNode);
                    neighborNode->setDepth(searchNode->getDepth()+1);
                }
//                if (neighborNode->getKind() == 0) {
//                    searchList.pushBack(neighborNode);
//                    neighborNode->setDepth(searchNode->getDepth()+1);
//                }
            }
            // ここまで+隣nodeまでの距離
            int cost = searchNode->getCost() + searchNode->getNeighborNodeDist().at(count);
            // 新規(1000)もしくは，最短なら登録
            if (neighborNode->getCost() > cost) {
//            log("隣の設定コスト%d", neighborNode->getCost());
//            log("新規コスト%d", cost);
                neighborNode->setCost(cost);
                neighborNode->setComeNode(searchNode);
//                log("  %dnodeは%dからきたとセット",neighborNode->getId(), searchNode->getId());
            }
            count ++;
        }
        searchList.erase(0);
    }
}
/**
 *  探索結果を再帰的に回収する
 *  @param route 初めstartを入れておく
 *  @param gId   ゴールid
 *  @return 経路
 */
Vector<NodeLayer*> BoardLayer::setRoute(Vector<NodeLayer*> route, int gId){
//    log("現在のルート");
//    for (auto r: route) {
//        log("r:%d",r->getId());
//    }
    // 行けるか確認
    if (!route.at(route.size()-1)->isComeNode()) {
//        log("どこからもこれません");
        Vector<NodeLayer*> tmp;
        return tmp;
    }
    // 末尾の来たnodeをaddする
    route.pushBack(route.at(route.size()-1)->getComeNode());
    // ゴール到着
    if (route.at(route.size()-1)->getId() == gId) {
        return route;
    }
    route = setRoute(route, gId);
    return route;
}

Vector<NodeLayer*> BoardLayer::getAllNode(){
    return allNode;
}