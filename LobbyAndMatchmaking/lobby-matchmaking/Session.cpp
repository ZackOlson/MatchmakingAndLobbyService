#include "Session.h"

std::function<std::shared_ptr<Session>(const std::string&)> Session::m_session_lookup;