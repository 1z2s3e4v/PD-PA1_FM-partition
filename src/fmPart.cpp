#include "fmDM.h"
#include<iostream>
#include<fstream>
#include <stdlib.h>
#include <time.h>
#include <iterator>
#include <algorithm>
//----------------------------------------------------------------------------------------------------------------------//
fmPart_C::fmPart_C(parser_C* pParser)
{
    m_BalanceDegree = pParser->getBalenceDegree();
    vector<net_C*> p_rvNet;
    pParser->getNet(p_rvNet);

    for(unsigned int i=0;i<p_rvNet.size();i++){
        vector<string> p_rvNode;
        p_rvNet[i]->getNode(p_rvNode);
        addNet(p_rvNet[i]->getName(),p_rvNode);
    }

    for(auto it = m_mapNode.begin();it != m_mapNode.end();++it){
        m_vNode.emplace_back(it->second);
    }
}
//----------------------------------------------------------------------------------------------------------------------//
void fmPart_C::addNet(string p_strNet, vector<string>& p_rvNode)
{
    fmNet_C *pNet = new fmNet_C(p_strNet);
    m_vNet.emplace_back(pNet);
    m_mapNet[p_strNet] = pNet;

    for(unsigned int j=0;j<p_rvNode.size();j++){
        string NodeName = p_rvNode[j];
        if(m_mapNode.find(NodeName) == m_mapNode.end()){ // new
            fmNode_C *p_pNode = new fmNode_C(NodeName);
            pNet->addNode(p_pNode);
            p_pNode->addNet(pNet);
            m_mapNode[NodeName] = p_pNode;
        }
        else{ // already in the map
            fmNode_C *p_pNode = m_mapNode[NodeName];
            pNet->addNode(p_pNode);
            p_pNode->addNet(pNet);
        }
    }
}
//----------------------------------------------------------------------------------------------------------------------//
bool fmPart_C::runFM(int last_minCut,clock_t start, double time_limit)
{ // return 1 --> in the time limit
    minCut = last_minCut;
    //cout << "\nrunFM():\n";
    savePart();
    //writeBestPart();
    for(unsigned int i=0;i<m_vNode.size();i++){// run all node
        if((double)(clock()-start)/CLOCKS_PER_SEC > time_limit){
            return 0;
        }

        int part;
        if((1-m_BalanceDegree)/2*m_mapNode.size()>mPartA_NodeNum){ // move B to A
            part = 1;
        }
        else if((1+m_BalanceDegree)/2*m_mapNode.size()<mPartA_NodeNum){ // move A to B
            part = 0;
        }
        else{ // move the node with largest gain 
            part = m_vMaxGain[0]>m_vMaxGain[1]?0:1;
        }

        if(m_vMaxGain[part] == -999999){// there is no more node to move
            break;
        }

        list<fmNode_C*>& _list = m_vBucket[part]->getList(m_vMaxGain[part]);
        if(_list.size()==0){
			return 1;
		}
        int r = rand() % _list.size();
        list<fmNode_C*>::iterator it_node = _list.begin(); // get the largest gain node
        for(int j=0;j<r;j++) it_node++;
        fmNode_C* rm_Node = *it_node;
        rm_Node->setLock(true);
        _list.erase(it_node); // erase it
        rm_Node->setIter(_list.end());
        moveNode(rm_Node);

        totalGain += rm_Node->getGain();
        if(totalGain > maxTotalGain ){
            int cut = getCut();
            maxTotalGain = totalGain;
            if(cut < minCut){
                minCut = cut;
                savePart();
                //cout << "cutsize: " << cut << "\n";
            }
        }

        //if(totalGain < 0) break;
    }
    return 1;
}
//----------------------------------------------------------------------------------------------------------------------//
void fmPart_C::onePass()
{
    
}
//----------------------------------------------------------------------------------------------------------------------//
void fmPart_C::initPart()
{
    //cout << "\ninitPart():\n";
    //put all node in partB
    for(unsigned int i=0;i<m_vNode.size();i++){
        m_vNode[i]->setPart(m_vNode[i]->ePartB);
        for(int j=0;j<m_vNode[i]->getNetNum();j++){
            m_vNode[i]->getNet(j)->addPart(1,1);
        }
    }

    //printInfo();
    m_vMaxGain.resize(2,-999999);
    calcGain();
    initBucket();
    buildBucket();
    
    // move half of partB to partA
    for(unsigned int i=0;i<m_vNode.size()/2;i++){
        list<fmNode_C*>& _list1 = m_vBucket[1]->getList(m_vMaxGain[1]);
        
        int r = rand() % _list1.size();
        list<fmNode_C*>::iterator it_node = _list1.begin(); // get the largest gain node
        for(int j=0;j<r;j++) it_node++;
        fmNode_C* mv_Node = *it_node;
        _list1.erase(it_node); // erase it
        mv_Node->setLock(true);
        moveNode(mv_Node);

        mv_Node->setLock(false);
        mv_Node->setGain(mv_Node->recalGain());
        list<fmNode_C*>& _list0 = m_vBucket[0]->getList(mv_Node->getGain());
        _list0.emplace_front(mv_Node);
        mv_Node->setIter(_list0.begin());
    }

    mPartA_NodeNum = m_vNode.size()/2;
    m_dBalance = (double)mPartA_NodeNum/m_vNode.size();
    
    //cout << "\ninitial balence: " << m_dBalance << "\n";
    //cout << "initial cutsize: " << getCut() << "\n\n";
}
//----------------------------------------------------------------------------------------------------------------------//
void fmPart_C::setPart(vector<string>&, vector<string>&)
{
    
}
//----------------------------------------------------------------------------------------------------------------------//
void fmPart_C::dump()
{
    
}
//----------------------------------------------------------------------------------------------------------------------//
void fmPart_C::refresh()
{
    
}
//----------------------------------------------------------------------------------------------------------------------//
int fmPart_C::getCut()
{
    int cutSize = 0;
    for(unsigned int i=0;i<m_vNet.size();i++){
        if(m_vNet[i]->getPart(0) > 0 && m_vNet[i]->getPart(1) > 0)
            cutSize++;
    }

    return cutSize;
}
//----------------------------------------------------------------------------------------------------------------------//
void fmPart_C::calcGain()
{
    for(unsigned int i=0;i<m_vNet.size();i++){
        // T(i)=0
        if(m_vNet[i]->getPart(0) == 0 || m_vNet[i]->getPart(1) == 0){
            for(int j=0;j<m_vNet[i]->getNodeNum();j++){
                m_vNet[i]->getNode(j)->addGain(-1);
            }
        }
        // F(i)=1
        else if(m_vNet[i]->getPart(0) == 1 || m_vNet[i]->getPart(1) == 1){
            // only two node in this net
            if(m_vNet[i]->getPart(0) == 1 && m_vNet[i]->getPart(1) == 1){
                m_vNet[i]->getNode(0)->addGain(1);
                m_vNet[i]->getNode(1)->addGain(1);
            }
            else{
                int almostPart = m_vNet[i]->getNode(0)->getPart();
                // node[0] and node[1] are in different group
                if(m_vNet[i]->getNode(1)->getPart() != almostPart){
                    // node[0] and node[2] are in different group
                    // so node[0] is the only node in other side
                    if(m_vNet[i]->getNode(2)->getPart() != almostPart)
                        m_vNet[i]->getNode(0)->addGain(1);
                    
                    // node[0] and node[2] are in same group
                    // so node[1] is the only node in other side
                    else
                        m_vNet[i]->getNode(1)->addGain(1);
                }
                else{
                    for(int j=1;j<m_vNet[i]->getNodeNum();j++){
                        // find the only node that in other side
                        if(m_vNet[i]->getNode(j)->getPart() != almostPart)
                            m_vNet[i]->getNode(j)->addGain(1);
                    }
                }
            }
        }  
    }
}
//----------------------------------------------------------------------------------------------------------------------//
void fmPart_C::calcPartNum(fmNet_C*,int&,int&,fmNode_C*&,fmNode_C*&)
{
    
}
//----------------------------------------------------------------------------------------------------------------------//
void fmPart_C::initBucket()
{   
    m_vBucket.clear();
    m_vBucket.resize(0);
    fmBucket_C* bucket1 = new fmBucket_C();
    fmBucket_C* bucket2 = new fmBucket_C();
    m_vBucket.emplace_back(bucket1);
    m_vBucket.emplace_back(bucket2);
}
//----------------------------------------------------------------------------------------------------------------------//
void fmPart_C::buildBucket()
{
    int maxNetNum = 0;
    for(unsigned int i=0;i<m_vNode.size();i++){
        if(m_vNode[i]->getNetNum() > maxNetNum)
            maxNetNum = m_vNode[i]->getNetNum();
    }
    m_vBucket[0]->setSize(maxNetNum);
    m_vBucket[1]->setSize(maxNetNum);
    
    for(unsigned int i=0;i<m_vNode.size();i++){
        list<fmNode_C*>& _list = m_vBucket[m_vNode[i]->getPart()]->getList(m_vNode[i]->getGain());
        _list.emplace_front(m_vNode[i]);
        m_vNode[i]->setIter(_list.begin());
    }
    
    updateMaxGain();
}
//----------------------------------------------------------------------------------------------------------------------//
bool fmPart_C::isBalance(int)
{
    int partA(0),partB(0);
    for(unsigned int i=0;i<m_vNet.size();i++){
        partA += m_vNet[i]->getPart(0);
        partB += m_vNet[i]->getPart(1);
    }
    return partA == partB;
}
//----------------------------------------------------------------------------------------------------------------------//
void fmPart_C::moveNode(fmNode_C* node)
{
    //cout << "move " << node->getName() << "\n";
    // PartA --> PartB
    if(node->getPart() == node->ePartA){
        for(int i=0;i<node->getNetNum();i++){
            fmNet_C* net = node->getNet(i);
            // there is only one node in partB before move
            if(net->getPart(1) == 1){ 
                for(int j=0;j<net->getNodeNum();j++){
                    if(!net->getNode(j)->isLock() && net->getNode(j)->getPart() == 1){
                        updateGain(net->getNode(j),-1);
                        break;
                    }
                }
            }
            else if(net->getPart(1) == 0){ // if partB has no node
                for(int j=0;j<net->getNodeNum();j++){
                    if(!net->getNode(j)->isLock()){ // all node gain +1
                        updateGain(net->getNode(j),1);
                        break;
                    }
                }
            }
            net->addPart(1,1);
            net->addPart(0,-1);
        }
        
        node->setPart(node->ePartB);
        mPartA_NodeNum--;
        m_dBalance = (double)mPartA_NodeNum/m_vNode.size();
    }
    // PartB --> PartA
    else {
        for(int i=0;i<node->getNetNum();i++){
            fmNet_C* net = node->getNet(i);
            if(net->getPart(0) == 1){ // there is only one node in partA
                for(int j=0;j<net->getNodeNum();j++){
                    if(!net->getNode(j)->isLock() && net->getNode(j)->getPart() == 0){
                        updateGain(net->getNode(j),-1);
                        break;
                    }
                }
            }
            else if(net->getPart(0) == 0){ // if partA has no node
                for(int j=0;j<net->getNodeNum();j++){
                    if(!net->getNode(j)->isLock()){ // all node gain +1
                        updateGain(net->getNode(j),1);
                    }
                }
            }
            net->addPart(1,-1);
            net->addPart(0,1);
        }
        
        node->setPart(node->ePartA);
        mPartA_NodeNum++;
        m_dBalance = (double)mPartA_NodeNum/m_vNode.size();
    }

    // update all node which is related with the node
    for(int j=0;j<node->getNetNum();j++){
        fmNet_C* net = node->getNet(j);

        // for partA and partB
        for(int part=0;part<2;part++){
            // part[i] become no node
            if(net->getPart(part) == 0){
                // all node gain-1
                for(int k=0;k<net->getNodeNum();k++){
                    fmNode_C* tmpNode = net->getNode(k);
                    if(!tmpNode->isLock()){
                        updateGain(tmpNode,-1);
                    }
                }
            }
            // part[i] become only one node
            else if(net->getPart(part) == 1){
                // the only node gain+1
                for(int k=0;k<net->getNodeNum();k++){
                    fmNode_C* tmpNode = net->getNode(k);
                    if(!tmpNode->isLock() && tmpNode->getPart() == part){
                        updateGain(tmpNode,1);
                        break;
                    }
                }
            }
        }
    }

    updateMaxGain();
}
//----------------------------------------------------------------------------------------------------------------------//
void fmPart_C::updateMaxIter(int,int,int&,int&)
{
    
}
//----------------------------------------------------------------------------------------------------------------------//
void fmPart_C::updateGain(fmNode_C* node, int change)
{
    // get the iterator and erase it
    list<fmNode_C*>& _list0 = m_vBucket[node->getPart()]->getList(node->getGain());
    list<fmNode_C*>::iterator it_node = node->getIter(); // get the node
    _list0.erase(it_node); // erase it
    
    // update gain
    node->addGain(change);
    list<fmNode_C*>& _list1 = m_vBucket[node->getPart()]->getList(node->getGain());
    _list1.emplace_front(node);
    node->setIter(_list1.begin());
}
//----------------------------------------------------------------------------------------------------------------------//
void fmPart_C::updateMaxGain()
{
    for(int i=m_vBucket[0]->getMax();i>=(-1)*m_vBucket[0]->getMax();--i){
        if(m_vBucket[0]->getList(i).size() > 0){
            m_vMaxGain[0] = i;
            break;
        }

        if(i ==(-1)*m_vBucket[0]->getMax()){
            m_vMaxGain[0] = -999999;
        }
    }
    for(int i=m_vBucket[1]->getMax();i>=(-1)*m_vBucket[1]->getMax();--i){
        if(m_vBucket[1]->getList(i).size() > 0){
            m_vMaxGain[1] = i;
            break;
        }

        if(i ==(-1)*m_vBucket[0]->getMax()){
            m_vMaxGain[0] = -999999;
        }
    }
}
//----------------------------------------------------------------------------------------------------------------------//
void fmPart_C::writeBestPart()
{
    ofstream outfile("output.txt", ifstream::out);
	if (!outfile.good()) {
		outfile.close();
		cout << "output.txt" << " write fail!!";
	}

    vector<string> v_g1;
    vector<string> v_g2;

    int count_g1=0, count_g2=0;
    for(auto it = m_mapNode.begin(); it != m_mapNode.end(); it++){
        //outfile << it->second->getBestPart() << "\n";
        if(it->second->getBestPart() == 0) {
            v_g1.emplace_back(it->second->getName());
            count_g1++;
        }
        else {
            v_g2.emplace_back(it->second->getName());
            count_g2++;
        }
    }
    cout << "Cutsize = " << minCut << "\n";
    cout << "G1 " << count_g1 << "\n";
    //for(string cellName : v_g1) cout << cellName << " ";
    //cout << ";\n";
    cout << "G2 " << count_g2 << "\n";
    //for(string cellName : v_g2) cout << cellName << " ";
    //cout << ";\n";

    outfile << "Cutsize = " << minCut << "\n";
    outfile << "G1 " << count_g1 << "\n";
    for(string cellName : v_g1) outfile << cellName << " ";
    outfile << ";\n";
    outfile << "G2 " << count_g2 << "\n";
    for(string cellName : v_g2) outfile << cellName << " ";
    outfile << ";\n";
}
void fmPart_C::writeBestPart(string outfileName)
{
    ofstream outfile(outfileName.c_str(), ifstream::out);
	if (!outfile.good()) {
		outfile.close();
		cout << "output.txt" << " write fail!!";
	}

    vector<string> v_g1;
    vector<string> v_g2;

    int count_g1=0, count_g2=0;
    for(auto it = m_mapNode.begin(); it != m_mapNode.end(); it++){
        //outfile << it->second->getBestPart() << "\n";
        if(it->second->getBestPart() == 0) {
            v_g1.emplace_back(it->second->getName());
            count_g1++;
        }
        else {
            v_g2.emplace_back(it->second->getName());
            count_g2++;
        }
    }
    cout << "Cutsize = " << minCut << "\n";
    cout << "G1 " << count_g1 << "\n";
    //for(string cellName : v_g1) cout << cellName << " ";
    //cout << ";\n";
    cout << "G2 " << count_g2 << "\n";
    //for(string cellName : v_g2) cout << cellName << " ";
    //cout << ";\n";

    outfile << "Cutsize = " << minCut << "\n";
    outfile << "G1 " << count_g1 << "\n";
    for(string cellName : v_g1) outfile << cellName << " ";
    outfile << ";\n";
    outfile << "G2 " << count_g2 << "\n";
    for(string cellName : v_g2) outfile << cellName << " ";
    outfile << ";\n";
}
//----------------------------------------------------------------------------------------------------------------------//
void fmPart_C::savePart()
{
    for(auto it = m_mapNode.begin(); it != m_mapNode.end(); it++){
        it->second->setBestPart(it->second->getPart());
    }
}
//----------------------------------------------------------------------------------------------------------------------//
void fmPart_C::restorePart(vector<fmNode_C::part_E>&)
{
    
}
//----------------------------------------------------------------------------------------------------------------------//
void fmPart_C::swapNode(vector<fmNode_C*>&,int)
{
    
}
//----------------------------------------------------------------------------------------------------------------------//
void fmPart_C::printInfo()
{
    cout << "[ printInfo() ]\n";
    for(unsigned int i=0;i<m_vNet.size();i++){
        cout << m_vNet[i]->getName() << ": ";
        cout << "size=" << m_vNet[i]->getNodeNum() << ", { ";
        for(int j=0;j<m_vNet[i]->getNodeNum();j++){
            cout << m_vNet[i]->getNode(j)->getName() << " ";
        }
        cout << "}\n";
    }

    /*cout << "\nget part for each net:\n";
    for(unsigned int i=0;i<m_vNet.size();i++){
        cout << m_vNet[i]->getName() << ": " << m_vNet[i]->getPart(0) << " " << m_vNet[i]->getPart(1) << "\n";
    }*/

    cout << "partA: ";
    for(unsigned int i=0;i<m_vNode.size();i++){
        if(m_vNode[i]->getPart() == 0)
            cout << m_vNode[i]->getName() << " ";
    }
    cout << "\npartB: ";
    for(unsigned int i=0;i<m_vNode.size();i++){
        if(m_vNode[i]->getPart() == 1)
            cout << m_vNode[i]->getName() << " ";
    }
    cout << "\n";

    /*cout << "\ngain:\n";
    for(unsigned int i=0;i<m_vNode.size();i++){
        cout << "node" << m_vNode[i]->getName() << ": " << m_vNode[i]->getGain() << "\n";  
    }*/
}
//----------------------------------------------------------------------------------------------------------------------//
void fmPart_C::dumpBucket(int)
{
    
}
//----------------------------------------------------------------------------------------------------------------------//
