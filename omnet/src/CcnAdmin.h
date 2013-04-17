/*
 * CcnAdmin.h
 *
 * Toolkit for parsing scenario files and configuring a ccn network
 *
 *  Created on: Sep 14, 2012
 *      Author: manolis sifalakis
 */

#ifndef CCN_ADMIN_H_
#define CCN_ADMIN_H_


#include <string>
#include "shared.h"




/*************************************************************************************************
 * Class CcnAdmin performs the configuration and administration of the CCN scenarios
 *
 * - Parses the scenario config file and stores the settings
 * - Configures the CCN nodes at init time with the pre-run setup
 * - At runtime it acts "Dark City" style transparently altering the state of the CCN
 *   network to reflect transient conditions
 *************************************************************************************************/



class CcnAdmin: public cSimpleModule
{
private:

    /**
     * Various possible scenario configuration options
     */
    enum ConfigType {
        eI_MODE = 0, SHOW_INTEREST = 0,
        ePC_MODE = 1, PRE_CACHE=1,
        eFR_MODE = 2, FWD_RULES=2
    };



    /**
     * Generic scenario configuration event data
     */
    typedef struct _ConfigRequest {
        _ConfigRequest  *next;

        int             nodeId;         // owner of the request (Ccn module ID)
        bool            isScheduled;    // whether a configuration event has been scheduled for execution
        bool            isExecuted;     // whether a configuration event has been executed
        cMessage        *event;         // ptr to event that will trigger the config action

        ConfigType      type;           // ConfigType enum value
        std::string     namedData;      // Named content prefix
        double          execTime;       // time of execution

        union {
            struct {
                cModule *nextHop;
                cModule *accessFrom;
            };

            struct {
                int     startChunk;     // starting chunk in PRE_CACHE
                int     numChunks;      // number of chunks in PRE_CACHE
            };
        };
    } ConfigRequest;





    /**
     * Registry of all the CCN modules in the omnet environment
     */
    typedef struct _NodeInfo {
        struct _NodeInfo    *next;
        struct _NodeInfo    *prev;
        int                 nodeId;         // getId() of Ccn module (NOT the node compound module); can also be arbitrary
        cModule             *nodePtr;       // address of Ccn module (NOT the node compound module)
        ConfigRequest       *configStart;
        ConfigRequest       *configEnd;
        int                 numConfigEvents;
    } NodeInfo;

    NodeInfo        *registryStart, *registryEnd;
    int             registrySize;



    int         debugLevel;



    /**
     * Utility functions for the Ccn Modules registry
     */
    NodeInfo *  getNodeInfo(int id);
    NodeInfo *  getNodeInfo(cModule *module);
    void        deleteNodeConfig(NodeInfo *node);
    void        handleMessage (cMessage *msg);



public:

    CcnAdmin ():
        registryStart(NULL),
        registryEnd(NULL),
        registrySize(0),
        debugLevel(0)
    {return;};
    //CcnAdmin () {return;};

    ~CcnAdmin ();


    /**
     * Initialization and termination of the module
     */
    virtual void    initialize();
    virtual void    finish() {return; };


    /**
     * Exported service functions (for direct method invocation by other modules)
     */
    bool registerCcnNode (cModule *node, int nodeId);
    bool unRegisterCcnNode (cModule *node, int nodeId);
    bool parseNodeConfig (cModule *node, const std::string &file);
    void scheduleConfigEvents (cModule *node);

    void dumpRegistry () {Enter_Method("CcnAdmin::dumpRegistry()"); return;};                 // TODO
    void dumpNodeConfig (int nodeId) {Enter_Method("CcnAdmin::dumpNodeConfig()"); return;};     // TODO

};




#endif /* CCN_ADMIN_H_ */
