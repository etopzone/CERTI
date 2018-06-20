// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002-2018  ISAE-SUPAERO & ONERA
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
			/* Ensure that the path is complete and ends per / (linux) or \\ (windows) */
			char last_character = path.back();
#ifdef WIN32
			char exp_last_character = '\\';
#else
			char exp_last_character = '/';
#endif
			if (last_character != exp_last_character)
			{
#ifdef WIN32
				filename = path +  "\\" + module;
#else
				filename = path +  "/" + module;
#endif
			}
			else
			{
				filename = path + module;
			}
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
            Debug(D, pdDebug) << "file successfully opened" << std::endl;
            try_to_open.close();
            return filename;
        }
    }

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

void parseModuleInto(const std::string& filepath, const FileType type, RootObject& result, const bool is_parsing_modules = false)
{
    std::ifstream fedFile(filepath);

    if (fedFile.is_open()) {
        fedFile.close();
        if (type == FileType::Fed) {
            // parse FED file and show the parse on stdout if parsing into the temporary root object
            int err = fedparser::build(filepath.c_str(), &result, !is_parsing_modules);
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
                    parser = make_unique<XmlParser2000>(&result, is_parsing_modules);
                    break;
                case XmlParser::XML_IEEE1516_2010:
                    parser = make_unique<XmlParser2010>(&result, is_parsing_modules);
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
}

void Federation::openFomModules(std::vector<std::string> modules, const bool is_mim)
{
#ifdef OUTPUT_ROOT_OBJECTS
    Debug(D, pdDebug) << "ROOT OBJECT" << std::endl;
    my_root_object->display();
#endif

    try {
        for (auto& module : modules) {
            Debug(D, pdDebug) << "Open module <" << module << ">" << std::endl;

            Debug(D, pdDebug) << "  Find file" << std::endl;
            auto module_path = filepathOf(module);

            Debug(D, pdDebug) << "  Check file type" << std::endl;
            auto file_type = checkFileType(module_path);

            Debug(D, pdDebug) << "  Parse file" << std::endl;
            auto temporary_root_object = RootObject{nullptr, true};
            parseModuleInto(module_path, file_type, temporary_root_object);

#ifdef OUTPUT_ROOT_OBJECTS
            Debug(D, pdDebug) << "TEMPORARY ROOT OBJECT" << std::endl;
            temporary_root_object.display();
#endif

            Debug(D, pdDebug) << "  Check consistency" << std::endl;
            if(is_mim) {
                auto is_compliant = Mom::isAvailableInRootObjectAndCompliant(temporary_root_object);
                if (!is_compliant) {
                    throw ErrorReadingFED("4.5.5.e : Invalid MIM.");
                }
            }
            else {
                auto is_valid = temporary_root_object.canBeAddedTo(*my_root_object);
                if (!is_valid) {
                    throw ErrorReadingFED("4.5.5.b : Invalid FOM module.");
                }
            }

            Debug(D, pdDebug) << "  Add to current root object" << std::endl;
            parseModuleInto(module_path, file_type, *my_root_object, true);

            Debug(D, pdDebug) << "  Update path to module" << std::endl;
            if(is_mim) {
                my_mim_module = module_path;
            }
            else {
                Debug(D, pdDebug) << my_fom_modules.size() << std::endl;
                my_fom_modules.push_back(module_path);
                Debug(D, pdDebug) << my_fom_modules.size() << std::endl;
            }

            Debug(D, pdDebug) << "  module successfully loaded from <" << module_path << ">" << std::endl;
        }
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
#ifdef OUTPUT_ROOT_OBJECTS
        Debug(D, pdDebug) << "ROOT OBJECT" << std::endl;
        my_root_object->display();
#endif

        Debug(D, pdExcept) << "Caught exception " << e.name() << " : " << e.reason() << std::endl;
        throw;
    }

#ifdef OUTPUT_ROOT_OBJECTS
        Debug(D, pdDebug) << "ROOT OBJECT" << std::endl;
        my_root_object->display();
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
}
}
