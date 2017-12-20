// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002-2005  ONERA
//
// This file is part of CERTI
//
// CERTI is free software ; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation ; either version 2 of the License, or
// (at your option) any later version.
//
// CERTI is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY ; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program ; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// ----------------------------------------------------------------------------

#include "Federation.hh"
#include <config.h>

// #include "NM_Classes.hh"
#include <algorithm>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
// #include <ext/alloc_traits.h>
#include <iostream>
#include <list>
#include <memory>
#include <set>
#include <sys/stat.h>
#include <time.h>
#include <utility>

#include <libCERTI/AuditFile.hh>
#include <libCERTI/Interaction.hh>
#include <libCERTI/InteractionSet.hh>
#include <libCERTI/LBTS.hh>
#include <libCERTI/MessageEvent.hh>
#include <libCERTI/NM_Classes.hh>
#include <libCERTI/NetworkMessage.hh>
#include <libCERTI/Object.hh>
#include <libCERTI/ObjectAttribute.hh>
#include <libCERTI/ObjectClass.hh>
#include <libCERTI/ObjectClassAttribute.hh>
#include <libCERTI/ObjectClassSet.hh>
#include <libCERTI/ObjectSet.hh>
#include <libCERTI/PrettyDebug.hh>
#include <libCERTI/RootObject.hh>
#include <libCERTI/SecurityServer.hh>
#include <libCERTI/SocketTCP.hh>
#include <libCERTI/XmlParser.hh>
#include <libCERTI/XmlParser2000.hh>
#include <libCERTI/XmlParser2010.hh>
#include <libCERTI/fed.hh>

#include <include/make_unique.hh>

// Definitions
#ifdef HAVE_XML
// #include <libxml/xmlmemory.h>
#include "libxml/xmlstring.h"
#include <libxml/parser.h>
// #include <libxml/tree.h>
#define ROOT_NODE (const xmlChar*) "rtigSaveData"
#define NODE_FEDERATION (const xmlChar*) "federation"
#define NODE_FEDERATE (const xmlChar*) "federate"
#endif // HAVE_XML

#if defined(_WIN32) && !defined(__MINGW32__)
#define strcasecmp stricmp
#endif

#if defined(_WIN32)
#define STAT_FUNCTION _stat
#define STAT_STRUCT struct _stat
#else
#define STAT_FUNCTION stat
#define STAT_STRUCT struct stat
#endif

namespace certi {

class RTIRegion;
class Socket;

namespace rtig {

static PrettyDebug D("FEDERATION", __FILE__);
static PrettyDebug G("GENDOC", __FILE__);
static PrettyDebug DNULL("RTIG_NULLMSG", "[RTIG NULL MSG]");

/** \defgroup certi_FOM_FileSearch CERTI FOM file search algorithm
 * When a federate calls the CreateFederationExcution API
 * RTIG tries to open FOM file from different predefined places,
 * using various environment variables:
 *
 * -# Bare filename considered as a path provided through <code> FEDid_name </code>
 * -# Use CERTI federation object model search PATH
 *    <code>getenv(CERTI_FOM_PATH) + FEDid_name</code>.
 *    <br><code>CERTI_FOM_PATH</code> environment variable may contains a list of path
 *    separated with ':'.
 * -# Using the <code> CERTI_HOME </code> environment variable
 *    <code>getenv(CERTI_HOME)+"/share/federations/"+ FEDid_name</code>
 * -# installation place plus <code>FEDid_name</code>
 *    <br><code>PACKAGE_INSTALL_PREFIX + "/share/federation/" + FEDid_name</code>
 * -# on Unix <code>"/usr/local/share/federation/" + FEDid_name</code>
 *    for backward compatibility reason.
 */

// Path splitting functions
std::vector<std::string> split(const std::string& s, char delim)
{
    std::vector<std::string> tokens;

    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        tokens.push_back(item);
    }
    return tokens;
}

/** Finds a module from its full path.
 * Throws exceptions if module cannot be found
 * We should try to open FOM file from different predefined places:
 * --> see doxygen doc at the top of this file.
 */
std::string filepathOf(const std::string& module)
{
    std::string filename = module;
    bool filefound = false;

    Debug(D, pdDebug) << "Looking for FOM file <" << filename << "> ... " << std::endl;

    STAT_STRUCT file_stat;
    filefound = (0 == STAT_FUNCTION(filename.c_str(), &file_stat));

    /* This is the main path handling loop */
    if (!filefound) {
        std::vector<std::string> fom_paths;

#ifdef WIN32
        char temp[260];
        GetCurrentDirectory(260, temp);
        fom_paths.push_back(std::string(temp) + "\\share\\federations\\");
#endif

        /* add paths from CERTI_FOM_PATH */
        if (getenv("CERTI_FOM_PATH")) {
            std::string path = getenv("CERTI_FOM_PATH");
            std::vector<std::string> certi_fom_paths = split(path, ':');
            fom_paths.insert(end(fom_paths), begin(certi_fom_paths), end(certi_fom_paths));
        }

        /* add path CERTI_HOME/share/federations/ */
        if (getenv("CERTI_HOME")) {
#ifdef WIN32
            fom_paths.push_back(std::string(getenv("CERTI_HOME")) + "\\share\\federations\\");
#else
            fom_paths.push_back(std::string(getenv("CERTI_HOME")) + "/share/federations/");
#endif
        }

#ifdef WIN32
        fom_paths.push_back(PACKAGE_INSTALL_PREFIX "\\share\\federations\\");
#else
        fom_paths.push_back(PACKAGE_INSTALL_PREFIX "/share/federations/");
        fom_paths.push_back("/usr/local/share/federations/");
#endif

        /* try to open FED using fom_paths prefixes */
        for (const std::string& path : fom_paths) {
            filename = path + module;
            filefound = (0 == STAT_FUNCTION(filename.c_str(), &file_stat));
            if (filefound) {
                break;
            }
        }
    }

    if (filefound) {
        // Try to open to verify if file exists
        std::ifstream try_to_open(filename);
        if (try_to_open.is_open()) {
            std::cout << "... opened." << std::endl;
            try_to_open.close();
            return filename;
        }
    }

    std::cout << "... failed : ";
    throw CouldNotOpenFED("Module not found");
}

enum class FileType { Fed, Xml };

FileType checkFileType(const std::string& filepath)
{
    // hope there is a . before fed or xml
    if (filepath.at(filepath.size() - 4) != '.') {
        throw CouldNotOpenFED("Incorrect extension (character '.' is missing)");
    }

    std::string extension = filepath.substr(filepath.size() - 3);

    Debug(D, pdTrace) << "filename is: " << filepath << " (extension is <" << extension << ">)" << std::endl;
    if (extension == "fed") {
        Debug(D, pdTrace) << "Trying to use .fed file" << std::endl;
        return FileType::Fed;
    }
    else if (extension == "xml") {
        Debug(D, pdTrace) << "Trying to use .xml file" << std::endl;
        return FileType::Xml;
    }
    else {
        throw CouldNotOpenFED("Incorrect extension (not .fed nor .xml)");
    }
}

RootObject parseModule(const std::string& filepath, const FileType type)
{
    RootObject result{RootObject::TemporaryRootObject()};

    std::ifstream fedFile(filepath);

    if (fedFile.is_open()) {
        fedFile.close();
        if (type == FileType::Fed) {
            // parse FED file and show the parse on stdout if verboseLevel>=2
            int err = fedparser::build(filepath.c_str(), &result, true);
            if (err != 0) {
                throw ErrorReadingFED("fed parser found error in FED file");
            }
        }
        else if (type == FileType::Xml) {
#ifdef HAVE_XML
            std::unique_ptr<XmlParser> parser;
            if (XmlParser::exists()) {
                switch (XmlParser::version(filepath)) {
                case XmlParser::XML_IEEE1516_2000:
                case XmlParser::XML_LEGACY:
                    parser = make_unique<XmlParser2000>(&result);
                    break;
                case XmlParser::XML_IEEE1516_2010:
                    parser = make_unique<XmlParser2010>(&result);
                    break;
                }

                parser->parse(filepath);
            }
            else
#endif
            {
                std::cerr << "CERTI was Compiled without XML support" << std::endl;
                Debug(G, pdGendoc) << "exit Federation::Federation on exception CouldNotOpenFED" << std::endl;
                throw CouldNotOpenFED("Could not parse XML file. (CERTI Compiled without XML lib.)");
            }
        }
    }

    return result;
}

void Federation::openMimModule()
{
    Debug(D, pdDebug) << "INITIAL ROOT OBJECT (must be empty)" << std::endl;
    my_root_object->display();
    
    try {
        Debug(D, pdDebug) << "Open mim module <" << my_mim_module << ">" << std::endl;
        
        // MIM can be provided without .xml extension
        Debug(D, pdDebug) << "  Find file" << std::endl;
        auto module_path = filepathOf(my_mim_module + ".xml");

        Debug(D, pdDebug) << "  Check file type" << std::endl;
        auto file_type = checkFileType(module_path);

        Debug(D, pdDebug) << "  Parse file" << std::endl;
        RootObject temporary_root_object = parseModule(module_path, file_type);
        
        Debug(D, pdDebug) << "  Module root object" << std::endl;
        temporary_root_object.display();

        Debug(D, pdDebug) << "  Check consistency" << std::endl;
        //         temporary_root_object.canBeAddedTo(my_root_object);
        auto is_compliant = Mom::isAvailableInRootObjectAndCompliant(temporary_root_object);
        if(!is_compliant) {
            throw ErrorReadingFED("4.5.5.e : Invalid MIM.");
        }

        Debug(D, pdDebug) << "  Add to current root object" << std::endl;
        //         temporary_root_object.insertInto(*my_root_object);

        Debug(D, pdDebug) << "  Update path to mim module" << std::endl;
        my_mim_module = module_path;
    }
    /*catch (CouldNotOpenFED& e) {
        Debug(D, pdExcept) << "Caught exception " << e.name() << " : " << e.reason() << std::endl;
        throw CouldNotOpenFED("4.5.5.f : Could not locate MIM indicated by supplied designator.");
    }
    catch (ErrorReadingFED& e) {
        Debug(D, pdExcept) << "Caught exception " << e.name() << " : " << e.reason() << std::endl;
        throw ErrorReadingFED("4.5.5.e : Invalid MIM.");
    }*/
    catch (Exception& e) {
        Debug(D, pdExcept) << "Caught exception " << e.name() << " : " << e.reason() << std::endl;
        throw;
    }
    
    Debug(D, pdDebug) << "ROOT OBJECT with module" << std::endl;
    my_root_object->display();
}

void Federation::openFomModules(const int verboseLevel)
{
    Debug(D, pdDebug) << "ROOT OBJECT" << std::endl;
    my_root_object->display();
    
    try {
        Debug(D, pdDebug) << "Open mim module <" << my_mim_module << ">" << std::endl;
        
        Debug(D, pdDebug) << "  Find file" << std::endl;
        auto module_path = filepathOf(my_mim_module);

        Debug(D, pdDebug) << "  Check file type" << std::endl;
        auto file_type = checkFileType(module_path);

        Debug(D, pdDebug) << "  Parse file" << std::endl;
        RootObject temporary_root_object = parseModule(module_path, file_type);
        
        Debug(D, pdDebug) << "TEMPORARY ROOT OBJECT" << std::endl;
        temporary_root_object.display();

        Debug(D, pdDebug) << "  Check consistency" << std::endl;
        //         temporary_root_object.canBeAddedTo(my_root_object);
        auto is_compliant = Mom::isAvailableInRootObjectAndCompliant(temporary_root_object);
        if(!is_compliant) {
            throw ErrorReadingFED("4.5.5.e : Invalid MIM.");
        }

        Debug(D, pdDebug) << "  Add to current root object" << std::endl;
        //         temporary_root_object.insertInto(*my_root_object);

        Debug(D, pdDebug) << "  Update path to mim module" << std::endl;
        my_mim_module = module_path;
        
        Debug(D, pdDebug) << "  MIM module successfully loaded from <" << my_mim_module << ">" << std::endl;
    }
    /*catch (CouldNotOpenFED& e) {
        Debug(D, pdExcept) << "Caught exception " << e.name() << " : " << e.reason() << std::endl;
        throw CouldNotOpenFED("4.5.5.f : Could not locate MIM indicated by supplied designator.");
    }
    catch (ErrorReadingFED& e) {
        Debug(D, pdExcept) << "Caught exception " << e.name() << " : " << e.reason() << std::endl;
        throw ErrorReadingFED("4.5.5.e : Invalid MIM.");
    }*/
    catch (Exception& e) {
        Debug(D, pdDebug) << "ROOT OBJECT" << std::endl;
        my_root_object->display();
        
        Debug(D, pdExcept) << "Caught exception " << e.name() << " : " << e.reason() << std::endl;
        throw;
    }
    
    Debug(D, pdDebug) << "ROOT OBJECT" << std::endl;
    my_root_object->display();
    
    
#if 0
    std::string filename = my_fom_modules.front();
    bool filefound = false;

    if (verboseLevel > 0) {
        std::cout << "Looking for FOM file <" << filename << "> ... " << std::endl;
        std::cout << "   Trying... " << filename;
    }

    STAT_STRUCT file_stat;
    filefound = (0 == STAT_FUNCTION(filename.c_str(), &file_stat));

    /* This is the main path handling loop */
    if (!filefound) {
        std::vector<std::string> fom_paths;
#ifdef WIN32
        char temp[260];
        GetCurrentDirectory(260, temp);
        fom_paths.insert(fom_paths.end(), std::string(temp) + "\\share\\federations\\");
#endif

        /* add paths from CERTI_FOM_PATH */
        if (getenv("CERTI_FOM_PATH")) {
            std::string path = getenv("CERTI_FOM_PATH");
            std::vector<std::string> certi_fom_paths = split(path, ':');
            fom_paths.insert(fom_paths.end(), certi_fom_paths.begin(), certi_fom_paths.end());
        }

        if (getenv("CERTI_HOME")) {
#ifdef WIN32
            fom_paths.insert(fom_paths.end(), std::string(getenv("CERTI_HOME")) + "\\share\\federations\\");
#else
            fom_paths.insert(fom_paths.end(), std::string(getenv("CERTI_HOME")) + "/share/federations/");
#endif
        }

#ifdef WIN32
        fom_paths.insert(fom_paths.end(), PACKAGE_INSTALL_PREFIX "\\share\\federations\\");
#else
        fom_paths.insert(fom_paths.end(), PACKAGE_INSTALL_PREFIX "/share/federations/");
        fom_paths.insert(fom_paths.end(), "/usr/local/share/federations/");
#endif

        /* try to open FED using fom_paths prefixes */
        for (const std::string& path : fom_paths) {
            if (verboseLevel > 0) {
                std::cout << " --> cannot access." << std::endl;
            }
            filename = path + my_fom_modules.front();
            if (verboseLevel > 0) {
                std::cout << "   Now trying... " << filename;
            }
            filefound = (0 == STAT_FUNCTION(filename.c_str(), &file_stat));
            if (filefound) {
                break;
            }
        }
    }

    if (!filefound) {
        if (verboseLevel > 0) {
            std::cout << " --> cannot access." << std::endl;
        }
        std::cerr << "Next step will fail, abort now" << std::endl;
        Debug(G, pdGendoc) << "exit Federation::Federation on exception CouldNotOpenFED" << std::endl;
        throw CouldNotOpenFED("RTIG cannot find FED file.");
    }

    // now really assign FEDid
    my_fom_modules.front() = filename;

    // Try to open to verify if file exists
    std::ifstream fedTry(my_fom_modules.front());
    if (!fedTry.is_open()) {
        if (verboseLevel > 0) {
            std::cout << "... failed : ";
        }
        Debug(G, pdGendoc) << "exit Federation::Federation on exception CouldNotOpenFED" << std::endl;
        throw CouldNotOpenFED("RTIG have found but cannot open FED file");
    }
    else {
        if (verboseLevel > 0) {
            std::cout << "... opened." << std::endl;
        }
        fedTry.close();
    }

    bool is_a_fed = false;
    bool is_an_xml = false;

    // hope there is a . before fed or xml
    if (filename.at(filename.size() - 4) != '.') {
        Debug(G, pdGendoc) << "exit Federation::Federation on exception CouldNotOpenFED" << std::endl;
        throw CouldNotOpenFED(
            "Incorrect FED file name, cannot find extension (character '.' is missing [or not in reverse 4th place])");
    }

    std::string extension = filename.substr(filename.size() - 3);

    Debug(D, pdTrace) << "filename is: " << filename << " (extension is <" << extension << ">)" << std::endl;
    if (extension == "fed") {
        is_a_fed = true;
        Debug(D, pdTrace) << "Trying to use .fed file" << std::endl;
    }
    else if (extension == "xml") {
        is_an_xml = true;
        Debug(D, pdTrace) << "Trying to use .xml file" << std::endl;
    }
    else {
        Debug(G, pdGendoc) << "exit Federation::Federation on exception CouldNotOpenFED" << std::endl;
        throw CouldNotOpenFED("Incorrect FED file name : nor .fed nor .xml file");
    }

    std::ifstream fedFile(filename);

    if (fedFile.is_open()) {
        fedFile.close();
        if (is_a_fed) {
            // parse FED file and show the parse on stdout if verboseLevel>=2
            int err = fedparser::build(filename.c_str(), my_root_object.get(), (verboseLevel >= 2));
            if (err != 0) {
                Debug(G, pdGendoc) << "exit Federation::Federation on exception ErrorReadingFED" << std::endl;
                throw ErrorReadingFED("fed parser found error in FED file");
            }

            // Retrieve the FED file last modification time(for Audit)
            STAT_STRUCT StatBuffer;
#if defined(_WIN32) && _MSC_VER >= 1400
            char MTimeBuffer[26];
#else
            char* MTimeBuffer;
#endif

            if (STAT_FUNCTION(filename.c_str(), &StatBuffer) == 0) {
#if defined(_WIN32) && _MSC_VER >= 1400
                ctime_s(&MTimeBuffer[0], 26, &StatBuffer.st_mtime);
#else
                MTimeBuffer = ctime(&StatBuffer.st_mtime);
#endif
                MTimeBuffer[strlen(MTimeBuffer) - 1] = 0; // Remove trailing \n
                my_server->audit << "(Last modified " << MTimeBuffer << ")";
            }
            else {
                my_server->audit << "(could not retrieve last modif time, errno " << errno << ").";
            }
        }
        else if (is_an_xml) {
#ifdef HAVE_XML
            std::unique_ptr<XmlParser> parser;
            if (XmlParser::exists()) {
                switch (XmlParser::version(filename)) {
                case XmlParser::XML_IEEE1516_2000:
                case XmlParser::XML_LEGACY:
                    parser = make_unique<XmlParser2000>(my_root_object.get());
                    break;
                case XmlParser::XML_IEEE1516_2010:
                    parser = make_unique<XmlParser2010>(my_root_object.get());
                    break;
                }
                my_server->audit << ", XML File : " << filename;

                try {
                    parser->parse(filename);
                }
                catch (Exception* e) {
                    throw;
                }
            }
            else
#endif
            {
                std::cerr << "CERTI was Compiled without XML support" << std::endl;
                Debug(G, pdGendoc) << "exit Federation::Federation on exception CouldNotOpenFED" << std::endl;
                throw CouldNotOpenFED("Could not parse XML file. (CERTI Compiled without XML lib.)");
            }
        }
    }
#endif
}

bool Federation::saveXmlData()
{
#ifndef HAVE_XML
    return false;
#else
    xmlDocPtr doc = xmlNewDoc((const xmlChar*) "1.0");
    doc->children = xmlNewDocNode(doc, NULL, ROOT_NODE, NULL);

    xmlNodePtr federation;
    federation = xmlNewChild(doc->children, NULL, NODE_FEDERATION, NULL);

    xmlSetProp(federation, (const xmlChar*) "name", (const xmlChar*) my_name.c_str());

    char t[10];
    sprintf(t, "%u", my_handle.get());
    xmlSetProp(federation, (const xmlChar*) "handle", (const xmlChar*) t);

    xmlNodePtr federateXmlNode;

    //     for (HandleFederateMap::iterator i = _handleFederateMap.begin(); i != _handleFederateMap.end(); ++i) {
    for (const auto& kv : my_federates) {
        federateXmlNode = xmlNewChild(federation, NULL, NODE_FEDERATE, NULL);

        xmlSetProp(federateXmlNode, (const xmlChar*) "name", (const xmlChar*) kv.second->getName().c_str());

        sprintf(t, "%u", kv.second->getHandle());
        xmlSetProp(federateXmlNode, (const xmlChar*) "handle", (const xmlChar*) t);

        xmlSetProp(federateXmlNode,
                   (const xmlChar*) "constrained",
                   (const xmlChar*) ((kv.second->isConstrained()) ? "true" : "false"));
        xmlSetProp(federateXmlNode,
                   (const xmlChar*) "regulator",
                   (const xmlChar*) ((kv.second->isRegulator()) ? "true" : "false"));
    }

    xmlSetDocCompressMode(doc, 9);

    std::string filename = my_name + "_" + my_save_label + ".xcs";
    xmlSaveFile(filename.c_str(), doc);

    // TODO: tests

    return true;
#endif // HAVE_XML
}

bool Federation::restoreXmlData(std::string docFilename)
{
#ifndef HAVE_XML
    (void) docFilename;
    return false;
#else

    xmlDocPtr doc = xmlParseFile(docFilename.c_str());

    // Did libXML manage to parse the file ?
    if (doc == 0) {
        std::cerr << "XML restore file not parsed successfully" << std::endl;
        xmlFreeDoc(doc);
        return false;
    }
    xmlNodePtr cur;

    cur = xmlDocGetRootElement(doc);
    if (cur == 0) {
        std::cerr << "XML file is empty" << std::endl;
        xmlFreeDoc(doc);
        return false;
    }

    // Is this root element an ROOT_NODE ?
    if (xmlStrcmp(cur->name, ROOT_NODE)) {
        std::cerr << "Wrong XML file: not the expected root node" << std::endl;
        return false;
    }

    cur = cur->xmlChildrenNode;
    if (xmlStrcmp(cur->name, NODE_FEDERATION)) {
        std::cerr << "Wrong XML file structure" << std::endl;
        return false;
    }

    if (strcmp(my_name.c_str(), XmlParser::CleanXmlGetProp(cur, (const xmlChar*) "name")) != 0) {
        std::cerr << "Wrong federation name" << std::endl;
    }

    cur = cur->xmlChildrenNode;

    bool status = false;
    while (cur != NULL) {
        if ((!xmlStrcmp(cur->name, NODE_FEDERATE))) {
            //             for (HandleFederateMap::iterator i = _handleFederateMap.begin(); i != _handleFederateMap.end(); ++i) {
            for (const auto& kv : my_federates) {
                if (!strcmp(kv.second->getName().c_str(), XmlParser::CleanXmlGetProp(cur, (const xmlChar*) "name"))) {
                    // Set federate constrained status
                    status = !strcmp("true", XmlParser::CleanXmlGetProp(cur, (const xmlChar*) "constrained"));

                    try {
                        kv.second->setConstrained(status);
                    }
                    catch (RTIinternalError& e) {
                        Debug(D, pdDebug) << "Federate was already constrained, no issue" << std::endl;
                    }

                    // Set federate regulating status
                    status = !strcmp("true", XmlParser::CleanXmlGetProp(cur, (const xmlChar*) "regulator"));

                    try {
                        kv.second->setRegulator(status);
                    }
                    catch (RTIinternalError& e) {
                        Debug(D, pdDebug) << "Federate was already regulator, no issue" << std::endl;
                    }

                    try {
                        kv.second->setHandle(strtol(XmlParser::CleanXmlGetProp(cur, (const xmlChar*) "handle"), 0, 10));
                    }
                    catch (RTIinternalError& e) {
                        Debug(D, pdDebug) << "Federate handle was already set, no issue" << std::endl;
                    }
                    break;
                }
            }
        }
        cur = cur->next;
    }

    return status;
#endif // HAVE_XML
}

Responses Federation::respondToAll(std::unique_ptr<NetworkMessage> message, const FederateHandle except)
{
    Responses responses;

    std::vector<Socket*> sockets;
    for (const auto& pair : my_federates) {
        if (pair.first != except) {
#ifdef HLA_USES_UDP
            sockets.push_back(my_server->getSocketLink(pair.first, BEST_EFFORT));
#else
            sockets.push_back(my_server->getSocketLink(pair.first));
#endif
        }
    }

    responses.emplace_back(sockets, std::move(message));

    return responses;
}
}
}
