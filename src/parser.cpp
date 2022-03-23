#include <fstream>
#include <cstring>
#include <cassert>
#include <sstream>
#include <algorithm>
#include <vector>
#include "parser.h"
using namespace std;

//----------------------------------------------------------------------------------------------------------------------//
bool parser_C::parse(char* p_pInput)
{
    // input
    ifstream input;
    input.open(p_pInput);
    printf("[PA1] - Parsing File:%s\n",p_pInput);
    // define
    net_C* pNet(0);
    string head, strNetName="", strNodeName, line="";
    // <Balance Degree>
    getline(input,line);
    balence_degree = stof(line);
    // Net <Net Name> [<Cell Name>]+ ;
    while(getline(input,line)){
        // check if line empty
        if(line == "") continue;
        // get line
        stringstream ss(line);
        // check if the head is 'NET', if yes than create new Net
        ss >> head;
        if(head == "NET"){
            ss >> strNetName;
            pNet = new net_C(strNetName);
            nNumNet++;
        }
        // get nodes
        while(ss >> strNodeName){
            if(strNodeName == ";") {strNetName = "";}
            if(strNetName == "" || strNodeName == ";" || strNodeName == "") continue;
            pNet->addNode(strNodeName);
            nNumNode++;
        }
        m_vNet.emplace_back(pNet);
    }
    printf("[PA1] - Num Net:%d\n",nNumNet);
    printf("[PA1] - Total Num Node:%d\n",nNumNode);
    return true;
}
//----------------------------------------------------------------------------------------------------------------------//
parser_C::~parser_C()
{
    for(unsigned int nNetIter=0;nNetIter<m_vNet.size();++nNetIter)
    {
        delete m_vNet[nNetIter];
    }
}
//----------------------------------------------------------------------------------------------------------------------//

