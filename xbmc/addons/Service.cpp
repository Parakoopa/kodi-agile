/*
 *      Copyright (C) 2005-2013 Team XBMC
 *      http://xbmc.org
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with XBMC; see the file COPYING.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */
#include "Service.h"
#include "AddonManager.h"
#include "interfaces/generic/ScriptInvocationManager.h"
#include "utils/log.h"
#include "system.h"

namespace ADDON
{

CService::CService(AddonInfoPtr addonInfo)
  : CAddon(addonInfo),
    m_type(UNKNOWN),
    m_startOption(LOGIN)
{
  std::string start = Type(ADDON_SERVICE)->GetValue("@start").asString();
  if (start == "startup")
    m_startOption = STARTUP;

  BuildServiceType();
}

bool CService::Start()
{
  bool ret = true;
  switch (m_type)
  {
#ifdef HAS_PYTHON
  case PYTHON:
    ret = (CScriptInvocationManager::GetInstance().ExecuteAsync(Type(ADDON_SERVICE)->LibPath(), this->shared_from_this()) != -1);
    break;
#endif

  case UNKNOWN:
  default:
    ret = false;
    break;
  }

  return ret;
}

bool CService::Stop()
{
  bool ret = true;

  switch (m_type)
  {
#ifdef HAS_PYTHON
  case PYTHON:
    ret = CScriptInvocationManager::GetInstance().Stop(Type(ADDON_SERVICE)->LibPath());
    break;
#endif

  case UNKNOWN:
  default:
    ret = false;
    break;
  }

  return ret;
}

void CService::BuildServiceType()
{
  std::string str = Type(ADDON_SERVICE)->LibPath();
  std::string ext;

  size_t p = str.find_last_of('.');
  if (p != std::string::npos)
    ext = str.substr(p + 1);

#ifdef HAS_PYTHON
  std::string pythonExt = ADDON_PYTHON_EXT;
  pythonExt.erase(0, 2);
  if ( ext == pythonExt )
    m_type = PYTHON;
  else
#endif
  {
    m_type = UNKNOWN;
    CLog::Log(LOGERROR, "ADDON: extension '%s' is not currently supported for service addon", ext.c_str());
  }
}

}
