#include "src/json/JSON.h"
#include "src/naming/beat/BeatInfo.h"
#include "NacosString.h"

/**
 * JSON
 *
 * @author Liu, Hanyu
 * Adapter from nacos-cpp-cli to a json parser
 */
using namespace std;
using namespace rapidjson;
using nacos::naming::Selector;

namespace nacos{
NacosString documentToString(const Document &d) {
    StringBuffer buffer;
    Writer <StringBuffer> writer(buffer);
    d.Accept(writer);
    NacosString result = buffer.GetString();
    return result;
}

NacosString valueToString(const Value &d) {
    StringBuffer buffer;
    Writer <StringBuffer> writer(buffer);
    d.Accept(writer);
    NacosString result = buffer.GetString();
    return result;
}

NacosString JSON::toJSONString(const map <NacosString, NacosString> &mapinfo) {
    Document d;
    d.SetObject();
    for (map<NacosString, NacosString>::const_iterator it = mapinfo.begin(); it != mapinfo.end(); it++) {
        Value k;
        k.SetString(it->first.c_str(), d.GetAllocator());
        Value v;
        v.SetString(it->second.c_str(), d.GetAllocator());
        d.AddMember(k, v, d.GetAllocator());
    }

    return documentToString(d);
}

void JSON::Map2JSONObject(Document &d, Value &jsonOb, map <NacosString, NacosString> &mapinfo) {
    jsonOb.SetObject();
    for (map<NacosString, NacosString>::iterator it = mapinfo.begin(); it != mapinfo.end(); it++) {
        Value k;
        k.SetString(it->first.c_str(), d.GetAllocator());
        Value v;
        v.SetString(it->second.c_str(), d.GetAllocator());
        jsonOb.AddMember(k, v, d.GetAllocator());
    }
}

void JSON::JSONObject2Map(std::map <NacosString, NacosString> &mapinfo, const Value &jsonOb) {
    for (Value::ConstMemberIterator iter = jsonOb.MemberBegin(); iter != jsonOb.MemberEnd(); ++iter) {
        NacosString name = iter->name.GetString();
        NacosString value = iter->value.GetString();
        mapinfo[name] = value;
    }
}

//Add key-value
void AddKV(Document &d, const NacosString &k, const NacosString &v) {
    Value k_, v_;
    k_.SetString(k.c_str(), d.GetAllocator());
    v_.SetString(v.c_str(), d.GetAllocator());
    d.AddMember(k_, v_, d.GetAllocator());
}

//Add key-Object
void AddKO(Document &d, const NacosString &k, Value &o) {
    Value k_;
    k_.SetString(k.c_str(), d.GetAllocator());
    d.AddMember(k_, o, d.GetAllocator());
}

NacosString JSON::toJSONString(BeatInfo &beatInfo) {
    Document d;
    d.SetObject();
    AddKV(d, "port", NacosStringOps::valueOf(beatInfo.port));
    AddKV(d, "ip", beatInfo.ip);
    AddKV(d, "weight", NacosStringOps::valueOf(beatInfo.weight));
    AddKV(d, "serviceName", beatInfo.serviceName);
    AddKV(d, "cluster", beatInfo.cluster);
    AddKV(d, "scheduled", NacosStringOps::valueOf(beatInfo.scheduled));
    Value metadata;
    Map2JSONObject(d, metadata, beatInfo.metadata);
    AddKO(d, "metadata", metadata);

    //d["port"] = NacosStringOps::valueOf(beatInfo.port);
    //d["ip"] = beatInfo.ip;
    //d["weight"] = NacosStringOps::valueOf(beatInfo.weight);
    //d["serviceName"] = beatInfo.serviceName;
    //d["cluster"] = beatInfo.cluster;
    //d["scheduled"] = beatInfo.scheduled;
    //d["metadata"] = toJSONString(beatInfo.metadata);
    return documentToString(d);
}

long JSON::getLong(const NacosString &jsonString, const NacosString &fieldname) {
    Document d;
    d.Parse(jsonString.c_str());
    Value &s = d[fieldname.c_str()];
    return s.GetInt64();
}

Instance JSON::Json2Instance(const Value &host) NACOS_THROW(NacosException) {
    Instance theinstance;

    if (host.HasMember("instanceId")) {
        const Value &instanceId = host["instanceId"];
        theinstance.instanceId = instanceId.GetString();
    }

    markRequired(host, "port");
    const Value &port = host["port"];
    if (!port.IsInt()) {
        throw NacosException(NacosException::INVALID_JSON_FORMAT, "Error while parsing port for Instance!");
    }
    theinstance.port = port.GetInt();

    markRequired(host, "ip");
    const Value &ip = host["ip"];
    theinstance.ip = ip.GetString();

    markRequired(host, "weight");
    const Value &weight = host["weight"];
    if (!weight.IsDouble()) {
        throw NacosException(NacosException::INVALID_JSON_FORMAT, "Error while parsing weight for Instance!");
    }
    theinstance.weight = weight.GetDouble();

    markRequired(host, "metadata");
    const Value &metadata = host["metadata"];

    std::map <NacosString, NacosString> mtdata;
    JSONObject2Map(mtdata, metadata);

    theinstance.metadata = mtdata;

    markRequired(host, "healthy");
    const Value &healthy = host["healthy"];
    if (!healthy.IsBool()) {
        throw NacosException(NacosException::INVALID_JSON_FORMAT, "Error while parsing healthy for Instance!");
    }
    theinstance.healthy = healthy.GetBool();

    markRequired(host, "enabled");
    const Value &enabled = host["enabled"];
    if (!enabled.IsBool()) {
        throw NacosException(NacosException::INVALID_JSON_FORMAT, "Error while parsing enabled for Instance!");
    }
    theinstance.enabled = enabled.GetBool();

    if (host.HasMember("clusterName")) {
        const Value &clusterName = host["clusterName"];
        theinstance.clusterName = clusterName.GetString();
    }

    return theinstance;
}

Instance JSON::Json2Instance(const NacosString &jsonString) NACOS_THROW(NacosException) {
    Document d;
    d.Parse(jsonString.c_str());

    Instance theinstance;

    markRequired(d, "instanceId");
    const Value &instanceId = d["instanceId"];
    theinstance.instanceId = instanceId.GetString();

    markRequired(d, "port");
    const Value &port = d["port"];
    if (!port.IsInt()) {
        throw NacosException(NacosException::INVALID_JSON_FORMAT, "Error while parsing port for Instance!");
    }
    theinstance.port = port.GetInt();

    markRequired(d, "ip");
    const Value &ip = d["ip"];
    theinstance.ip = ip.GetString();

    markRequired(d, "weight");
    const Value &weight = d["weight"];
    if (!weight.IsDouble()) {
        throw NacosException(NacosException::INVALID_JSON_FORMAT, "Error while parsing weight for Instance!");
    }
    theinstance.weight = weight.GetDouble();

    markRequired(d, "metadata");
    const Value &metadata = d["metadata"];

    std::map <NacosString, NacosString> mtdata;
    JSONObject2Map(mtdata, metadata);
    theinstance.metadata = mtdata;

    markRequired(d, "healthy");
    const Value &healthy = d["healthy"];
    if (!healthy.IsBool()) {
        throw NacosException(NacosException::INVALID_JSON_FORMAT, "Error while parsing healthy for Instance!");
    }
    theinstance.healthy = healthy.GetBool();

    markRequired(d, "service");
    const Value &service = d["service"];
    theinstance.serviceName = service.GetString();

    markRequired(d, "clusterName");
    const Value &clusterName = d["clusterName"];
    theinstance.clusterName = clusterName.GetString();

    return theinstance;
}

void JSON::markRequired(const Document &d, const NacosString &requiredField) NACOS_THROW(NacosException) {
    if (!d.HasMember(requiredField.c_str())) {
        throw NacosException(NacosException::LACK_JSON_FIELD, "Missing required field:" + requiredField);
    }
}

void JSON::markRequired(const Value &v, const NacosString &requiredField) NACOS_THROW(NacosException) {
    if (!v.HasMember(requiredField.c_str())) {
        throw NacosException(NacosException::LACK_JSON_FIELD, "Missing required field:" + requiredField);
    }
}

ServiceInfo JSON::JsonStr2ServiceInfo(const NacosString &jsonString) NACOS_THROW(NacosException) {
    ServiceInfo si;
    Document d;
    d.Parse(jsonString.c_str());

    if (d.HasParseError()) {
        throw NacosException(NacosException::INVALID_JSON_FORMAT,
                             "Error while parsing the JSON String for ServiceInfo!");
    }

    markRequired(d, "hosts");
    const Value &hosts = d["hosts"];
    if (hosts.Size() == 0) {
        return si;
    }
    std::list <Instance> hostlist;
    for (SizeType i = 0; i < hosts.Size(); i++) {
        const Value &curhost = hosts[i];
        Instance curinstance = Json2Instance(curhost);
        hostlist.push_back(curinstance);
    }

    si.setHosts(hostlist);

    markRequired(d, "cacheMillis");
    const Value &cacheMillis = d["cacheMillis"];
    if (!cacheMillis.IsInt64()) {
        throw NacosException(NacosException::INVALID_JSON_FORMAT, "Error while parsing cacheMillis for ServiceInfo!");
    }
    si.setCacheMillis(cacheMillis.GetInt64());

    markRequired(d, "checksum");
    const Value &checkSum = d["checksum"];
    si.setChecksum(checkSum.GetString());

    markRequired(d, "lastRefTime");
    const Value &lastRefTime = d["lastRefTime"];
    if (!lastRefTime.IsInt64()) {
        throw NacosException(NacosException::INVALID_JSON_FORMAT, "Error while parsing lastRefTime for ServiceInfo!");
    }
    si.setLastRefTime(lastRefTime.GetInt64());

    markRequired(d, "clusters");
    const Value &clusters = d["clusters"];
    si.setClusters(clusters.GetString());

    markRequired(d, "name");
    const Value &name = d["name"];
    ServiceInfo::fromKey(si, name.GetString());

    return si;
}

NacosServerInfo parseOneNacosSvr(const Value &curSvr) {
    NacosServerInfo res;
    res.setIp(curSvr["ip"].GetString());
    res.setPort(curSvr["servePort"].GetInt());
    res.setSite(curSvr["site"].GetString());
    res.setWeight(curSvr["weight"].GetFloat());
    res.setAdWeight(curSvr["adWeight"].GetFloat());
    res.setAlive(curSvr["alive"].GetBool());
    res.setLastRefTime(curSvr["lastRefTime"].GetInt64());
    if (!curSvr["lastRefTimeStr"].IsNull()) {
        res.setLastRefTimeStr(curSvr["lastRefTimeStr"].GetString());
    }
    res.setKey(curSvr["key"].GetString());
    return res;
}

list <NacosServerInfo> JSON::Json2NacosServerInfo(const NacosString &nacosString) NACOS_THROW(NacosException) {
    list <NacosServerInfo> nacosServerList;
    ServiceInfo si;
    Document d;
    d.Parse(nacosString.c_str());

    if (d.HasParseError()) {
        throw NacosException(NacosException::INVALID_JSON_FORMAT,
                             "Error while parsing the JSON String for NacosServerInfo!");
    }

    markRequired(d, "servers");
    const Value &servers = d["servers"];
    if (!servers.IsArray()) {
        throw NacosException(NacosException::INVALID_JSON_FORMAT, "Error while parsing servers for NacosServerInfo!");
    }

    for (SizeType i = 0; i < servers.Size(); i++) {
        const Value &curSvr = servers[i];
        NacosServerInfo curSvrInfo = parseOneNacosSvr(curSvr);
        nacosServerList.push_back(curSvrInfo);
    }

    return nacosServerList;
}

ListView<NacosString> JSON::Json2ServiceList(const NacosString &nacosString) NACOS_THROW(NacosException) {
    ListView<NacosString> serviceList;
    ServiceInfo si;
    Document d;
    d.Parse(nacosString.c_str());

    if (d.HasParseError()) {
        throw NacosException(NacosException::INVALID_JSON_FORMAT,
                             "Error while parsing the JSON String for ServiceList!");
    }

    markRequired(d, "count");
    markRequired(d, "doms");
    const Value &count = d["count"];
    if (!count.IsInt()) {
        throw NacosException(NacosException::INVALID_JSON_FORMAT, "Error while parsing servers for ServiceList.count!");
    }

    serviceList.setCount(count.GetInt());

    const Value &doms = d["doms"];
    if (!doms.IsArray()) {
        throw NacosException(NacosException::INVALID_JSON_FORMAT, "Error while parsing servers for ServiceList.doms!");
    }

    list<NacosString> names;

    for (SizeType i = 0; i < doms.Size(); i++) {
        const Value &curName = doms[i];
        names.push_back(curName.GetString());
    }
    serviceList.setData(names);

    return serviceList;
}

map<NacosString, NacosString> parseMetadata(const Value &value) {
    map<NacosString, NacosString> metadata;

    for (Value::ConstMemberIterator iter = value.MemberBegin();
        iter != value.MemberEnd(); ++iter){
        metadata[iter->name.GetString()] = iter->value.GetString();
    }

    return metadata;
}

ServiceInfo2 JSON::Json2ServiceInfo2(const NacosString &nacosString) NACOS_THROW(NacosException) {
    ServiceInfo2 serviceInfo2;
    Document d;
    d.Parse(nacosString.c_str());

    markRequired(d, "groupName");
    markRequired(d, "namespaceId");
    markRequired(d, "name");
    const Value &groupName = d["groupName"];
    const Value &namespaceId = d["namespaceId"];
    const Value &name = d["name"];
    serviceInfo2.setGroupName(groupName.GetString());
    serviceInfo2.setNamespaceId(namespaceId.GetString());
    serviceInfo2.setName(name.GetString());

    const Value &selector = d["selector"];

    serviceInfo2.setSelector(valueToString(selector));

    markRequired(d, "protectThreshold");
    const Value &protectThreshold = d["protectThreshold"];
    serviceInfo2.setProtectThreshold(protectThreshold.GetDouble());

    //service info metadata
    const Value &serviceMetadata = d["metadata"];
    if (!serviceMetadata.IsObject()) {
        throw NacosException(NacosException::INVALID_JSON_FORMAT, "Error while parsing metadata for ServiceInfo2!");
    }

    serviceInfo2.setMetadata(parseMetadata(serviceMetadata));

    const Value &clusterlist = d["clusters"];
    if (!clusterlist.IsArray()) {
        throw NacosException(NacosException::INVALID_JSON_FORMAT, "Error while parsing clusters for ServiceInfo2.clusters!");
    }

    //cluster metadata
    for (SizeType i = 0; i < clusterlist.Size(); i++) {
        const Value &curClusterJson = clusterlist[i];
        Cluster curCluster;
        curCluster.setName(curClusterJson["name"].GetString());
        curCluster.setMetadata(parseMetadata(curClusterJson["metadata"]));
        HealthChecker healthChecker;
        curCluster.setHealthChecker(healthChecker);
    }

    return serviceInfo2;
}

AccessToken JSON::Json2AccessToken(const NacosString &nacosString) NACOS_THROW(NacosException)
{
    AccessToken accessTokenRes;
    Document d;
    d.Parse(nacosString.c_str());
    markRequired(d, "accessToken");
    const Value &accessToken = d["accessToken"];
    accessTokenRes.accessToken = accessToken.GetString();
    markRequired(d, "tokenTtl");
    const Value &tokenTtl = d["tokenTtl"];
    accessTokenRes.tokenTtl = tokenTtl.GetInt();
    markRequired(d, "globalAdmin");
    const Value &globalAdmin = d["globalAdmin"];
    accessTokenRes.globalAdmin = globalAdmin.GetBool();

    return accessTokenRes;
}

PushPacket JSON::Json2PushPacket(const char *jsonString) NACOS_THROW(NacosException)
{
    PushPacket pushPacket;
    Document d;
    d.Parse(jsonString);
    markRequired(d, "data");
    const Value &data = d["data"];
    pushPacket.data = data.GetString();

    markRequired(d, "type");
    const Value &type = d["type"];
    pushPacket.type = type.GetString();

    markRequired(d, "lastRefTime");
    const Value &lastRefTime = d["lastRefTime"];
    pushPacket.lastRefTime = lastRefTime.GetInt64();

    return pushPacket;
}

}//namespace nacos