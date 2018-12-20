/******************************************************************************\
Copyright (c) 2005-2015, Intel Corporation
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

This sample was distributed or derived from the Intel's Media Samples package.
The original version of this sample may be obtained from https://software.intel.com/en-us/intel-media-server-studio
or https://software.intel.com/en-us/media-client-solutions-support.
\**********************************************************************************/

#pragma once

#ifndef __PLUGIN_LOADER_H__
#define __PLUGIN_LOADER_H__

#include "libdefine.h"
#include <iostream>
#include <iomanip> // for std::setfill, std::setw
#include <memory> // for std::auto_ptr

void msdk_so_free(msdk_so_handle handle);

msdk_so_handle msdk_so_load(const TCHAR *file_name);

msdk_func_pointer msdk_so_get_addr(msdk_so_handle handle, const char *func_name);

bool AreGuidsEqual(const mfxPluginUID& guid1, const mfxPluginUID& guid2);


class MsdkSoModule
{
protected:
    msdk_so_handle m_module;
public:
    MsdkSoModule()
        : m_module(NULL)
    {
    }
    MsdkSoModule(const msdk_string & pluginName)
        : m_module(NULL)
    {
        m_module = msdk_so_load(pluginName.c_str());
    }
    template <class T>
    T GetAddr(const std::string & fncName)
    {
        return reinterpret_cast<T>(msdk_so_get_addr(m_module, fncName.c_str()));;
    }

    virtual ~MsdkSoModule()
    {
        if (m_module)
        {
            msdk_so_free(m_module);
            m_module = NULL;
        }
    }
};

/*
* Rationale: class to load+register any mediasdk plugin decoder/encoder/generic by given name
*/
class PluginLoader : public MFXPlugin
{
protected:
    mfxPluginType     ePluginType;

    mfxSession        m_session;
    mfxPluginUID      m_uid;

private:
    const TCHAR* msdkGetPluginName(const mfxPluginUID& guid)
    {
        if (AreGuidsEqual(guid, MFX_PLUGINID_HEVCD_SW))
            return MSDK_STRING("Intel (R) Media SDK plugin for HEVC DECODE");
        else if(AreGuidsEqual(guid, MFX_PLUGINID_HEVCD_HW))
            return MSDK_STRING("Intel (R) Media SDK HW plugin for HEVC DECODE");
        else if(AreGuidsEqual(guid, MFX_PLUGINID_HEVCE_SW))
            return MSDK_STRING("Intel (R) Media SDK plugin for HEVC ENCODE");
        else if(AreGuidsEqual(guid, MFX_PLUGINID_HEVCE_HW))
            return MSDK_STRING("Intel (R) Media SDK HW plugin for HEVC ENCODE");
        else if(AreGuidsEqual(guid, MFX_PLUGINID_H264LA_HW))
            return MSDK_STRING("Intel (R) Media SDK plugin for LA ENC");
        else if(AreGuidsEqual(guid, MFX_PLUGINID_ITELECINE_HW))
            return MSDK_STRING("Intel (R) Media SDK PTIR plugin (HW)");
        else
            return MSDK_STRING("Unknown plugin");
    }

public:
    PluginLoader(mfxPluginType type, mfxSession session, const mfxPluginUID & uid, mfxU32 version, const mfxChar *pluginName, mfxU32 len)
        : ePluginType(type)
        , m_session()
        , m_uid()
    {
        len; pluginName;
        mfxStatus sts = MFX_ERR_NONE;
        msdk_stringstream strStream;

        MSDK_MEMCPY(&m_uid, &uid, sizeof(mfxPluginUID));
        for (size_t i = 0; i != sizeof(mfxPluginUID); i++)
        {
            strStream << MSDK_STRING("0x") << std::setfill(MSDK_CHAR('0')) << std::setw(2) << std::hex << (int)m_uid.Data[i];
            if (i != (sizeof(mfxPluginUID)-1)) strStream << MSDK_STRING(", ");
        }

        if ((ePluginType == MFX_PLUGINTYPE_AUDIO_DECODE) ||
            (ePluginType == MFX_PLUGINTYPE_AUDIO_ENCODE))
        {
            // Audio plugins are not loaded by path
            sts = MFX_ERR_UNSUPPORTED;
        }
        else
        {
            sts = MFXVideoUSER_LoadByPath(session, &m_uid, version, pluginName, len);
        }

        if (MFX_ERR_NONE == sts){
            m_session = session;
        }
    }

    PluginLoader(mfxPluginType type, mfxSession session, const mfxPluginUID & uid, mfxU32 version)
        : ePluginType(type)
        , m_session()
        , m_uid()
    {
        mfxStatus sts = MFX_ERR_NONE;
        msdk_stringstream strStream;

        MSDK_MEMCPY(&m_uid, &uid, sizeof(mfxPluginUID));
        for (size_t i = 0; i != sizeof(mfxPluginUID); i++)
        {
            strStream << MSDK_STRING("0x") << std::setfill(MSDK_CHAR('0')) << std::setw(2) << std::hex << (int)m_uid.Data[i];
            if (i != (sizeof(mfxPluginUID)-1)) strStream << MSDK_STRING(", ");
        }

        if ((ePluginType == MFX_PLUGINTYPE_AUDIO_DECODE) ||
            (ePluginType == MFX_PLUGINTYPE_AUDIO_ENCODE))
        {
            sts = MFXAudioUSER_Load(session, &m_uid, version);
        }
        else
        {
            sts = MFXVideoUSER_Load(session, &m_uid, version);
        }

        if (MFX_ERR_NONE == sts)
        {
            m_session = session;
        }
    }

    virtual ~PluginLoader()
    {
        mfxStatus sts = MFX_ERR_NONE;
        if (m_session)
        {
            if ((ePluginType == MFX_PLUGINTYPE_AUDIO_DECODE) ||
                (ePluginType == MFX_PLUGINTYPE_AUDIO_ENCODE))
            {
                sts = MFXAudioUSER_UnLoad(m_session, &m_uid);
            }
            else
            {
                sts = MFXVideoUSER_UnLoad(m_session, &m_uid);
            }
        }
    }

    bool IsOk() {
        return m_session != 0;
    }
    virtual mfxStatus PluginInit( mfxCoreInterface *core ) {
        core;
        return MFX_ERR_NULL_PTR;
    }
    virtual mfxStatus PluginClose() {
        return MFX_ERR_NULL_PTR;
    }
    virtual mfxStatus GetPluginParam( mfxPluginParam *par ) {
        par;
        return MFX_ERR_NULL_PTR;
    }
    virtual mfxStatus Execute( mfxThreadTask task, mfxU32 uid_p, mfxU32 uid_a ) {
        task; uid_p; uid_a;
        return MFX_ERR_NULL_PTR;
    }
    virtual mfxStatus FreeResources( mfxThreadTask task, mfxStatus sts ) {
        task; sts;
        return MFX_ERR_NULL_PTR;
    }
    virtual void Release() {
    }
    virtual mfxStatus Close() {
        return MFX_ERR_NULL_PTR;
    }
    virtual mfxStatus SetAuxParams( void* auxParam, int auxParamSize ) {
        auxParam; auxParamSize;
        return MFX_ERR_NULL_PTR;
    }
};

inline MFXPlugin * LoadPluginByType(mfxPluginType type, mfxSession session, const mfxPluginUID & uid, mfxU32 version, const mfxChar *pluginName, mfxU32 len) {
    std::auto_ptr<PluginLoader> plg(new PluginLoader (type, session, uid, version, pluginName, len));
    return plg->IsOk() ? plg.release() : NULL;
}

inline MFXPlugin * LoadPluginByGUID(mfxPluginType type, mfxSession session, const mfxPluginUID & uid, mfxU32 version) {
    std::auto_ptr<PluginLoader> plg(new PluginLoader (type, session, uid, version));
    return plg->IsOk() ? plg.release() : NULL;
}

inline MFXPlugin * LoadPlugin(mfxPluginType type, mfxSession session, const mfxPluginUID & uid, mfxU32 version, const mfxChar *pluginName, mfxU32 len) {
    return LoadPluginByType(type, session, uid, version, pluginName, len);
}

inline MFXPlugin * LoadPlugin(mfxPluginType type, mfxSession session, const mfxPluginUID & uid, mfxU32 version) {
    return LoadPluginByGUID(type, session, uid, version);
}
#endif // PLUGIN_LOADER