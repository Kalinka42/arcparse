/********************************
 * ToDo:
 * - Process data?
 ********************************/

#include <cstddef>
#include <cstdint>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdint>
#include <vector>
#include <zconf.h>
#include <archive.h>
#include <archive_entry.h>


struct CombatEvent {
    uint64_t time;
    uint64_t src_agent;
    uint64_t dst_agent;
    int32_t value;
    int32_t buff_dmg;
    uint32_t overstack_value;
    uint32_t skillid;
    uint16_t src_instid;
    uint16_t dst_instid;
    uint16_t src_master_instid;
    uint16_t dst_master_instid;
    uint8_t iff;
    uint8_t buff;
    uint8_t result;
    uint8_t is_activation;
    uint8_t is_buffremove;
    uint8_t is_ninety;
    uint8_t is_fifty;
    uint8_t is_moving;
    uint8_t is_statechange;
    uint8_t is_flanking;
    uint8_t is_shields;
    uint8_t is_offcycle;
    uint8_t pad61;
    uint8_t pad62;
    uint8_t pad63;
    uint8_t pad64;
};

struct Header {
    char date[13];
    uint8_t revision;
    short int bossID;
    uint8_t speciesId;
};

struct Agent {
    uint64_t addr;
    uint32_t prof;
    uint32_t is_Elite;
    uint16_t toughness;
    uint16_t concentration;
    uint16_t healing;
    uint16_t hitbox_width;
    uint16_t condition;
    uint16_t hitbox_height;
    char name[68];
};

struct Skill {
    int32_t id;
    char name[64];
};

bool jsonOutput = false;
bool genOutput = false;
uint32_t skillCount;
uint32_t agentCount;

void evaluateArgument(char*);
const void generateJsonFile(const Header&, const std::vector<Agent>&, const std::vector<Skill>&,
                            const std::vector<CombatEvent>&, const std::string&);
Header getHeaderInfo(std::stringstream&);
std::vector<Agent> getAgentInfo(std::stringstream&);
Agent readAgent(std::stringstream&);
std::vector<Skill> getSkillInfo(std::stringstream&);
Skill readSkill(std::stringstream&);
std::vector<CombatEvent> getCombatEventsInfo(std::stringstream&);
CombatEvent readCombatEvent(std::stringstream&);
void parseLog(std::stringstream&, Header&, std::vector<Agent>&, std::vector<Skill>&, std::vector<CombatEvent>&);
bool loadFileToStream(const char*, std::stringstream&);
bool logIsZipped(const char*);
const void generateOutput(const Header&, const std::vector<Agent>&, const std::vector<Skill>&, const std::vector<CombatEvent>&);
bool extractArchiveToStream(const char* , std::stringstream&);
const std::string headerJson(const Header&);
const std::string agentsJson(const std::vector<Agent>&);
const std::string skillsJson(const std::vector<Skill>&);
const std::string combatEventsJson(const std::vector<CombatEvent>&);
std::string escapeJsonString(const std::string&);

int main(int argc, char *argv[]) {
    if(argc < 2){
        std::cerr << "Error: Please enter log name" << std::endl;
        return 0;
    }
    for(int i = 2; i < argc; i++) {
        evaluateArgument(argv[i - 1]);
    }
    const char* logFile = argv[argc - 1];
    std::string logName(logFile);

    std::vector<Agent> agents;
    std::vector<Skill> skills;
    std::vector<CombatEvent> combatevts;
    Header headerInfo;
    std::stringstream buffer;

    if(logIsZipped(logFile)) {
        extractArchiveToStream(logFile, buffer);
    } else {
        loadFileToStream(logFile, buffer);
    }
    parseLog(buffer, headerInfo, agents, skills, combatevts);
    
    if(jsonOutput) {
        generateJsonFile(headerInfo, agents, skills, combatevts, logName);
    }
    if(genOutput) {
        generateOutput(headerInfo, agents, skills, combatevts);
    }
    return 0;
}

const std::string combatEventsJson(const std::vector<CombatEvent>& cbtevts){
    std::stringstream ss;
    ss << "[";

    for (size_t i = 0; i < cbtevts.size(); i++) {
        const CombatEvent& cbtevt = cbtevts[i];
        ss << "{";
        ss << "\"time\":" << cbtevt.time << ",";
        ss << "\"src_agent\":" << cbtevt.src_agent << ",";
        ss << "\"dst_agent\":" << cbtevt.dst_agent << ",";
        ss << "\"value\":" << cbtevt.value << ",";
        ss << "\"buff_dmg\":" << cbtevt.buff_dmg << ",";
        ss << "\"overstack_value\":" << cbtevt.overstack_value << ",";
        ss << "\"skillid\":" << cbtevt.skillid << ",";
        ss << "\"src_instid\":" << cbtevt.src_instid << ",";
        ss << "\"dst_instid\":" << cbtevt.dst_instid << ",";
        ss << "\"src_master_instid\":" << cbtevt.src_master_instid << ",";
        ss << "\"dst_master_instid\":" << cbtevt.dst_master_instid << ",";
        ss << "\"iff\":" << static_cast<int>(cbtevt.iff) << ",";
        ss << "\"buff\":" << static_cast<int>(cbtevt.buff) << ",";
        ss << "\"result\":" << static_cast<int>(cbtevt.result) << ",";
        ss << "\"is_activation\":" << static_cast<int>(cbtevt.is_activation) << ",";
        ss << "\"is_buffremove\":" << static_cast<int>(cbtevt.is_buffremove) << ",";
        ss << "\"is_ninety\":" << static_cast<int>(cbtevt.is_ninety) << ",";
        ss << "\"is_fifty\":" << static_cast<int>(cbtevt.is_fifty) << ",";
        ss << "\"is_moving\":" << static_cast<int>(cbtevt.is_moving) << ",";
        ss << "\"is_statechange\":" << static_cast<int>(cbtevt.is_statechange) << ",";
        ss << "\"is_flanking\":" << static_cast<int>(cbtevt.is_flanking) << ",";
        ss << "\"is_shields\":" << static_cast<int>(cbtevt.is_shields) << ",";
        ss << "\"is_offcycle\":" << static_cast<int>(cbtevt.is_offcycle) << ",";
        ss << "\"pad61\":" << static_cast<int>(cbtevt.pad61) << ",";
        ss << "\"pad62\":" << static_cast<int>(cbtevt.pad62) << ",";
        ss << "\"pad63\":" << static_cast<int>(cbtevt.pad63) << ",";
        ss << "\"pad64\":" << static_cast<int>(cbtevt.pad64);
        ss << "}";

        if (i != cbtevts.size() - 1) {
            ss << ",";
        }
    }

    ss << "]";

    return ss.str();
}
const std::string skillsJson(const std::vector<Skill>& skills) {
    std::stringstream ss;
    ss << "[";

    for (size_t i = 0; i < skills.size(); i++) {
        const Skill& skill = skills[i];
        ss << "{";
        ss << "\"id\":" << skill.id << ",";
        ss << "\"name\":\"" << escapeJsonString(std::string(skill.name)) << "\"";
        ss << "}";

        if (i != skills.size() - 1) {
            ss << ",";
        }
    }
    ss << "]";
    return ss.str();
}

std::string escapeJsonString(const std::string& str) {
    std::stringstream ss;
    for (const char c : str) {
        switch (c) {
            case '\"': ss << "\\\""; break;
            default: ss << c;
        }
    }
    return ss.str();
}

const std::string agentsJson(const std::vector<Agent>& agents) {
    std::stringstream ss;
    ss << "[";

    for (size_t i = 0; i < agents.size(); i++) {
        const Agent& agent = agents[i];
        ss << "{";
        ss << "\"addr\":" << agent.addr << ",";
        ss << "\"prof\":" << agent.prof << ",";
        ss << "\"is_Elite\":" << agent.is_Elite << ",";
        ss << "\"toughness\":" << agent.toughness << ",";
        ss << "\"concentration\":" << agent.concentration << ",";
        ss << "\"healing\":" << agent.healing << ",";
        ss << "\"hitbox_width\":" << agent.hitbox_width << ",";
        ss << "\"condition\":" << agent.condition << ",";
        ss << "\"hitbox_height\":" << agent.hitbox_height << ",";
        ss << "\"name\":\"" << agent.name << "\"";
        ss << "}";

        if (i != agents.size() - 1) {
            ss << ",";
        }
    }
    ss << "]";

    return ss.str();
}

const std::string headerJson(const Header& header) {
    std::stringstream ss;
    ss << "{";
    ss << "\"date\":\"" << header.date << "\",";
    ss << "\"revision\":" << static_cast<int>(header.revision) << ",";
    ss << "\"bossID\":" << header.bossID << ",";
    ss << "\"speciesId\":" << static_cast<int>(header.speciesId);
    ss << "}";

    std::string output = ss.str();
    return output;
}

bool extractArchiveToStream(const char* zipFileName, std::stringstream& outputStream) {
    struct archive* archive = archive_read_new();
    archive_read_support_filter_all(archive);
    archive_read_support_format_all(archive);
    int r = archive_read_open_filename(archive, zipFileName, 10240);
    if (r != ARCHIVE_OK) {
        std::cerr << "Failed to open zip archive: " << zipFileName << std::endl;
        archive_read_free(archive);
        return false;
    }

    struct archive_entry* entry;
    while (archive_read_next_header(archive, &entry) == ARCHIVE_OK) {
        if (archive_entry_size(entry) > 0) {
            char* buffer = new char[archive_entry_size(entry)];
            int bytesRead = archive_read_data(archive, buffer, archive_entry_size(entry));
            if (bytesRead < 0) {
                std::cerr << "Failed to read entry data for " << archive_entry_pathname(entry) << std::endl;
                delete[] buffer;
                archive_read_close(archive);
                archive_read_free(archive);
                return false;
            }
            outputStream.write(buffer, archive_entry_size(entry));
            delete[] buffer;
        }
    }

    archive_read_close(archive);
    archive_read_free(archive);

    return true;
}

bool loadFileToStream(const char* filename, std::stringstream& outputStream) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cout << "Failed to open file." << std::endl;
        return false;
    }

    outputStream << file.rdbuf();

    file.close();
    return true;
}

bool logIsZipped(const char* filename) {
    std::string filenameStr(filename);
    if(filenameStr.find(".zevtc") == std::string::npos) {
        return false;
    }
    return true;
}

void parseLog(std::stringstream& stream, Header& header, std::vector<Agent>& agents, 
              std::vector<Skill>& skills, std::vector<CombatEvent>& cbtevts) {
    stream.seekg(0);
    header = getHeaderInfo(stream);
    stream.read(reinterpret_cast<char *>(&agentCount), sizeof(agentCount));
    agents = getAgentInfo(stream);
    stream.read(reinterpret_cast<char *>(&skillCount), sizeof(skillCount));
    skills = getSkillInfo(stream);
    cbtevts = getCombatEventsInfo(stream);
}


const void generateJsonFile(const Header& header, const std::vector<Agent>& agents,
                            const std::vector<Skill>& skills, const std::vector<CombatEvent>& cbtevts, const std::string& filename) {
    std::stringstream ss;
    ss << "{";
    //Header Json Output
    ss << "\"Header\":" << headerJson(header) << ",";
    //Agents Json Output
    ss << "\"Agent Count\":" << agentCount << ",";
    ss << "\"Agents\":" << agentsJson(agents) << ",";
    //Skill Json Output
    ss << "\"Skill Count\":" << skillCount << ",";
    ss << "\"Skills\":" << skillsJson(skills) << ",";
    //CombatEvents Json Output
    ss << "\"Combat Events\":" << combatEventsJson(cbtevts);
    ss << "}";

    std::string jsonfile = filename.substr(0, filename.length() - 5);
    jsonfile += ".json";
    std::ofstream file(jsonfile);
    file << ss.str();
    std::cout << "Exported data to file: " << jsonfile << std::endl;
}

void evaluateArgument(char* arg){
    std::string argStr(arg);
    if(argStr == "-j") {
        jsonOutput = true;
    } else if(argStr == "-i") {
        genOutput = true;
    }
}

const void generateOutput(const Header& headerInfo, const std::vector<Agent>& agents,
                          const std::vector<Skill>& skills, const std::vector<CombatEvent>& cbtevts){
    std::string date(headerInfo.date, headerInfo.date + sizeof(headerInfo.date));
    std::cout << "Date: " << date << std::endl;
    std::cout << "Revision: " << static_cast<unsigned int>(headerInfo.revision) << std::endl;
    std::cout << "Boss ID: " << headerInfo.bossID << std::endl;
    std::cout << "Species ID: " << static_cast<unsigned int>(headerInfo.speciesId) << std::endl;
    std::cout << "Num. of Agents: " << agentCount << std::endl;
    std::cout << "Skill Count: " << skillCount << std::endl;
    int k = 1;
    for(const CombatEvent ev : cbtevts) {
        k++;
    }
    std::cout << "Number of Combat Events: " << k << std::endl;
}

Header getHeaderInfo(std::stringstream& file){
    Header header;
    file.read(header.date, 12);
    header.date[12] = 0; 
    file.read(reinterpret_cast<char *>(&header.revision), sizeof(header.revision));
    file.read(reinterpret_cast<char *>(&header.bossID), sizeof(header.bossID));
    file.read(reinterpret_cast<char *>(&header.speciesId), sizeof(header.speciesId));
    return header;
}

Agent readAgent(std::stringstream& file){
    Agent tempAgent;
    file.read(reinterpret_cast<char *>(&tempAgent.addr), sizeof(tempAgent.addr));
    file.read(reinterpret_cast<char *>(&tempAgent.prof), sizeof(tempAgent.prof));
    file.read(reinterpret_cast<char *>(&tempAgent.is_Elite), sizeof(tempAgent.is_Elite));
    file.read(reinterpret_cast<char *>(&tempAgent.toughness), sizeof(tempAgent.toughness));
    file.read(reinterpret_cast<char *>(&tempAgent.concentration), sizeof(tempAgent.concentration));
    file.read(reinterpret_cast<char *>(&tempAgent.healing), sizeof(tempAgent.healing));
    file.read(reinterpret_cast<char *>(&tempAgent.hitbox_width), sizeof(tempAgent.hitbox_width));
    file.read(reinterpret_cast<char *>(&tempAgent.condition), sizeof(tempAgent.condition));
    file.read(reinterpret_cast<char *>(&tempAgent.hitbox_height), sizeof(tempAgent.hitbox_height));
    file.read(tempAgent.name, 68);
    return tempAgent;
}

std::vector<Agent> getAgentInfo(std::stringstream& file){
    std::vector<Agent> agents;
    for(int i = 0; i < agentCount; i++){
        agents.push_back(readAgent(file));
    }
    return agents;
}

Skill readSkill(std::stringstream& file){
    Skill tempSkill;
    file.read(reinterpret_cast<char *>(&tempSkill.id), sizeof(tempSkill.id));
    file.read(tempSkill.name, sizeof(tempSkill.name));
    return tempSkill;
}

std::vector<Skill> getSkillInfo(std::stringstream& file){
    std::vector<Skill> skills;
    for(int i = 0; i < skillCount; i++){
        skills.push_back(readSkill(file));
    }
    return skills;
}

std::vector<CombatEvent> getCombatEventsInfo(std::stringstream& file) {
    std::vector<CombatEvent> cbtevts;
    while(file){
        cbtevts.push_back(readCombatEvent(file));
    }
    return cbtevts;
}

CombatEvent readCombatEvent(std::stringstream& file){
    CombatEvent cbtevt;
    file.read(reinterpret_cast<char *>(&cbtevt.time), sizeof(cbtevt.time));
    file.read(reinterpret_cast<char *>(&cbtevt.src_agent), sizeof(cbtevt.src_agent));
    file.read(reinterpret_cast<char *>(&cbtevt.dst_agent), sizeof(cbtevt.dst_agent));
    file.read(reinterpret_cast<char *>(&cbtevt.value), sizeof(cbtevt.value));
    file.read(reinterpret_cast<char *>(&cbtevt.buff_dmg), sizeof(cbtevt.buff_dmg));
    file.read(reinterpret_cast<char *>(&cbtevt.overstack_value), sizeof(cbtevt.overstack_value));
    file.read(reinterpret_cast<char *>(&cbtevt.skillid), sizeof(cbtevt.skillid));
    file.read(reinterpret_cast<char *>(&cbtevt.src_instid), sizeof(cbtevt.src_instid));
    file.read(reinterpret_cast<char *>(&cbtevt.dst_instid), sizeof(cbtevt.dst_instid));
    file.read(reinterpret_cast<char *>(&cbtevt.src_master_instid), sizeof(cbtevt.src_master_instid));
    file.read(reinterpret_cast<char *>(&cbtevt.dst_master_instid), sizeof(cbtevt.dst_master_instid));
    file.read(reinterpret_cast<char *>(&cbtevt.iff), sizeof(cbtevt.iff));
    file.read(reinterpret_cast<char *>(&cbtevt.buff), sizeof(cbtevt.buff));
    file.read(reinterpret_cast<char *>(&cbtevt.result), sizeof(cbtevt.result));
    file.read(reinterpret_cast<char *>(&cbtevt.is_activation), sizeof(cbtevt.is_activation));
    file.read(reinterpret_cast<char *>(&cbtevt.is_buffremove), sizeof(cbtevt.is_buffremove));
    file.read(reinterpret_cast<char *>(&cbtevt.is_ninety), sizeof(cbtevt.is_ninety));
    file.read(reinterpret_cast<char *>(&cbtevt.is_fifty), sizeof(cbtevt.is_fifty));
    file.read(reinterpret_cast<char *>(&cbtevt.is_moving), sizeof(cbtevt.is_moving));
    file.read(reinterpret_cast<char *>(&cbtevt.is_statechange), sizeof(cbtevt.is_statechange));
    file.read(reinterpret_cast<char *>(&cbtevt.is_flanking), sizeof(cbtevt.is_flanking));
    file.read(reinterpret_cast<char *>(&cbtevt.is_shields), sizeof(cbtevt.is_shields));
    file.read(reinterpret_cast<char *>(&cbtevt.is_offcycle), sizeof(cbtevt.is_offcycle));
    file.read(reinterpret_cast<char *>(&cbtevt.pad61), sizeof(cbtevt.pad61));
    file.read(reinterpret_cast<char *>(&cbtevt.pad62), sizeof(cbtevt.pad62));
    file.read(reinterpret_cast<char *>(&cbtevt.pad63), sizeof(cbtevt.pad63));
    file.read(reinterpret_cast<char *>(&cbtevt.pad64), sizeof(cbtevt.pad64));
    return cbtevt;
}







