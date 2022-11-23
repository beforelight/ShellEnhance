#include "Agent.h"
namespace Agent {
    Agent &Agent::instance() {
        static Agent AgentOnlyOne;
        return AgentOnlyOne;
    }
    std::vector<std::shared_ptr<Agent::Object> > Agent::findByType(const std::string &type) {
        std::vector<std::shared_ptr<Object> > rtv;
        for (const auto &i: m_objs) {
            if (i->Type() == type)
                rtv.push_back(i);
        }
        return rtv;
    }
    std::vector<std::shared_ptr<Agent::Object> > Agent::findById(const std::string &id) {
        std::vector<std::shared_ptr<Object> > rtv;
        for (const auto &i: m_objs) {
            if (i->Id() == id)
                rtv.push_back(i);
        }
        return rtv;
    }
    std::shared_ptr<Agent::Object> Agent::find(const std::string &id, const std::string &type) {
        for (auto &i: m_objs) {
            if (i->Id() == id && i->Type() == type)
                return i;
        }
        return nullptr;
    }
    std::shared_ptr<Agent::Object> Agent::append(std::shared_ptr<Object> obj) {
        m_objs.push_back(obj);
        return obj;
    }

} // namespace Agent
