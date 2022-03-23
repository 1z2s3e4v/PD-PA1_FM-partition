#ifndef _PARSER_H_
#define _PARSER_H_
#include <vector>
#include <string>
using namespace std;
class net_C;

//----------------------------------------------------------------------------------------------------------------------//
class parser_C
{
    public:
        //Constructor
        parser_C() {};
        ~parser_C();
        //Main API
        bool parse(char*);

        //Accessor
        void getNet(vector<net_C*>& p_rvNet) {p_rvNet = m_vNet;}
        int getNumNet() {return nNumNet;}
        int getNumNode() {return nNumNode;}
        double getBalenceDegree() {return balence_degree;}
    private:
        vector<net_C*> m_vNet;
        int nNumNet = 0;
        int nNumNode = 0;
        double balence_degree = 0.5;
};
//----------------------------------------------------------------------------------------------------------------------//
class net_C
{
    public:
        //Constructor
        net_C(string p_strName) : m_strName(p_strName) {}

        //Accesor
        string getName() {return m_strName;}
        void getNode(vector<string>& p_rvNode) {p_rvNode = m_vNode;}
        //Modifier
        void addNode(string p_strNode) {m_vNode.emplace_back(p_strNode);}
    private:
        vector<string> m_vNode;        
        string m_strName; 
};
//----------------------------------------------------------------------------------------------------------------------//
#endif

