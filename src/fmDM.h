#include <vector>
#include <map>
#include <string>
#include <set>
#include <list>
#include "parser.h"

using std::string;
using std::vector;
using std::map;
using std::set;
using std::list;
class fmNet_C;
class fmNode_C;

//----------------------------------------------------------------------------------------------------------------------//
class fmNode_C
{
    public:
        // Enumerator
        enum part_E {ePartA=0, ePartB=1, ePartC=2, ePartD=3};

        // Constructor
        fmNode_C(string p_strName);
        // Accessor
        string getName();
        part_E getPart();
        part_E getBestPart();
        int    getGain();
        int    recalGain();
        bool   isLock();
        int    getNetNum();
        fmNet_C* getNet(int);
        list<fmNode_C*>::iterator getIter();
        // Modifier
        void addGain(int);
        void addNet(fmNet_C*);
        void setPart(part_E);
        void setBestPart(part_E);
        void setGain(int);
        void setLock(bool);
        void setIter(list<fmNode_C*>::iterator);

    private:
        int m_nGain;
        string m_strName;
        vector<fmNet_C*> m_vNet;
        part_E m_ePart;
        bool m_bLock;
        bool m_bSwap;
        list<fmNode_C*>::iterator m_itList;
        part_E m_eBestPart;
};
//----------------------------------------------------------------------------------------------------------------------//
class fmNet_C
{
    public:
        // Constructor
        fmNet_C(string m_strName);
        // Accessor
        string    getName();
        int       getNetWeight();
        int       getNodeNum();
        fmNode_C* getNode(int);
        int       getPart(int);
        // Modifier
        void addNode(fmNode_C*);
        void setPart(int,int);
        void addPart(int,int);
    private:
        string m_strName;
        vector<fmNode_C*> m_vNode;
        int m_nNetWeight;
        vector<int> m_vPart;
};
//----------------------------------------------------------------------------------------------------------------------//
class fmBucket_C
{
    public:
        fmBucket_C();
        ~fmBucket_C();
        list<fmNode_C*>& getList(int p_nId) {return p_nId<0?getNeg(p_nId*(-1)):getPos(p_nId);}
        list<fmNode_C*>& getNeg(int p_nId);
        list<fmNode_C*>& getPos(int p_nId);
        void clear();
        void setSize(int);
        int getMax() {return m_nMax;}
    private:
        vector< list<fmNode_C*> > m_vPositive;
        vector< list<fmNode_C*> > m_vNegative;
        int m_nMax;
};
//----------------------------------------------------------------------------------------------------------------------//
class fmPart_C
{
    public:
        // Constructor
        fmPart_C(parser_C*);
        void addNet(string p_strNet, vector<string>& p_rvNode);
        
        //
        bool runFM(int,clock_t,double);
        void onePass();
        // Function
        void initPart();
        void setPart(vector<string>&, vector<string>&);
        void dump();

        void refresh();
        int  getCut();
        void calcGain();
        void calcPartNum(fmNet_C*,int&,int&,fmNode_C*&,fmNode_C*&);
        void initBucket();
        void buildBucket();
        bool isBalance(int);
        void moveNode(fmNode_C*);
        void updateMaxIter(int,int,int&,int&);
        void updateGain(fmNode_C*, int);
        void updateMaxGain();
        void writeBestPart();
        void writeBestPart(string);
        void savePart();
        void restorePart(vector<fmNode_C::part_E>&);
        void swapNode(vector<fmNode_C*>&,int);
        void printInfo();
        int getMinCut(){return minCut;}
        //
        void dumpBucket(int);
    private:
        //
        map<string,fmNode_C*> m_mapNode;
        map<string,fmNet_C*>  m_mapNet;
        //
        vector<fmNode_C*>        m_vNode;
        vector<fmNet_C*>         m_vNet;
        vector<fmBucket_C*>      m_vBucket;
        vector<int>              m_vMaxGain; // the largest gain of the part
        vector<int>              m_vSize;
        double m_dBalance; // A/all
        int mPartA_NodeNum = 0;
        int totalGain = 0;
        int maxTotalGain = -99999;
        int minCut = 999999;
        double m_BalanceDegree = 0.5;
};
//----------------------------------------------------------------------------------------------------------------------//

