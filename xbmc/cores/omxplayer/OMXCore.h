#pragma once
/*
 *      Copyright (C) 2010 Team XBMC
 *      http://www.xbmc.org
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
 *  along with XBMC; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *  http://www.gnu.org/copyleft/gpl.html
 *
 */

#if defined(HAVE_LIBOPENMAX)

#include "utils/StdString.h"

#include <queue>

// TODO: should this be in configure
#ifndef OMX_SKIP64BIT
#define OMX_SKIP64BIT
#endif

#include "DllOMX.h"

////////////////////////////////////////////////////////////////////////////////////////////
// debug spew defines
#if 0
#define OMX_DEBUG_VERBOSE
#define OMX_DEBUG_EVENTHANDLER
#endif

#define OMX_INIT_STRUCTURE(a) \
  memset(&(a), 0, sizeof(a)); \
  (a).nSize = sizeof(a); \
  (a).nVersion.s.nVersionMajor = OMX_VERSION_MAJOR; \
  (a).nVersion.s.nVersionMinor = OMX_VERSION_MINOR; \
  (a).nVersion.s.nRevision = OMX_VERSION_REVISION; \
  (a).nVersion.s.nStep = OMX_VERSION_STEP

#define INPUT_BUFFER_SIZE 1024*512

#include "DllAvFormat.h"

typedef struct omx_event {
  OMX_EVENTTYPE eEvent;
  OMX_U32 nData1;
  OMX_U32 nData2;
} omx_event;

typedef struct omx_codec_capability {
    // level is OMX_VIDEO_AVCPROFILETYPE, OMX_VIDEO_H263PROFILETYPE, 
    // or OMX_VIDEO_MPEG4PROFILETYPE depending on context.
    OMX_U32 level;
    // level is OMX_VIDEO_AVCLEVELTYPE, OMX_VIDEO_H263LEVELTYPE, 
    // or OMX_VIDEO_MPEG4PROFILETYPE depending on context.
    OMX_U32 profile;
} omx_codec_capability;

class DllLibOMXCore;

class COMXCore;
class COMXCoreComponent;
class COMXCoreTunel;
class COMXCoreClock;

class COMXCoreTunel
{
public:
  COMXCoreTunel();
  ~COMXCoreTunel();

  void Initialize(COMXCoreComponent *src_component, unsigned int src_port, COMXCoreComponent *dst_component, unsigned int dst_port);
  OMX_ERRORTYPE Flush();
  OMX_ERRORTYPE Deestablish();
  OMX_ERRORTYPE Establish(bool portSettingsChanged);
private:
  bool              m_portSettingsChanged;
  COMXCoreComponent *m_src_component;
  COMXCoreComponent *m_dst_component;
  unsigned int      m_src_port;
  unsigned int      m_dst_port;
  DllOMX            *m_DllOMX;
  bool              m_DllOMXOpen;
};

class COMXCoreComponent
{
public:
  COMXCoreComponent();
  ~COMXCoreComponent();

  OMX_HANDLETYPE    GetComponent()   { return m_handle;        };
  unsigned int      GetInputPort()   { return m_input_port;    };
  unsigned int      GetOutputPort()  { return m_output_port;   };
  CStdString        GetName()        { return m_componentName; };

  OMX_ERRORTYPE DisableAllPorts();
  void          Remove(OMX_EVENTTYPE eEvent, OMX_U32 nData1, OMX_U32 nData2);
  OMX_ERRORTYPE AddEvent(OMX_EVENTTYPE eEvent, OMX_U32 nData1, OMX_U32 nData2);
  OMX_ERRORTYPE WaitForEvent(OMX_EVENTTYPE event);
  OMX_ERRORTYPE WaitForCommand(OMX_U32 command, OMX_U32 nData2);
  OMX_ERRORTYPE SetStateForComponent(OMX_STATETYPE state);
  OMX_STATETYPE GetState();
  OMX_ERRORTYPE SetParameter(OMX_INDEXTYPE paramIndex, OMX_PTR paramStruct);
  OMX_ERRORTYPE GetParameter(OMX_INDEXTYPE paramIndex, OMX_PTR paramStruct);
  OMX_ERRORTYPE SetConfig(OMX_INDEXTYPE configIndex, OMX_PTR configStruct);
  OMX_ERRORTYPE GetConfig(OMX_INDEXTYPE configIndex, OMX_PTR configStruct);
  OMX_ERRORTYPE SendCommand(OMX_COMMANDTYPE cmd, OMX_U32 cmdParam, OMX_PTR cmdParamData);
  bool          Initialize( const CStdString &component_name, OMX_INDEXTYPE index);
  bool          Deinitialize();

  // OMXCore Decoder delegate callback routines.
  static OMX_ERRORTYPE DecoderEventHandlerCallback(OMX_HANDLETYPE hComponent, OMX_PTR pAppData,
    OMX_EVENTTYPE eEvent, OMX_U32 nData1, OMX_U32 nData2, OMX_PTR pEventData);
  static OMX_ERRORTYPE DecoderEmptyBufferDoneCallback(
    OMX_HANDLETYPE hComponent, OMX_PTR pAppData, OMX_BUFFERHEADERTYPE* pBuffer);
  static OMX_ERRORTYPE DecoderFillBufferDoneCallback(
    OMX_HANDLETYPE hComponent, OMX_PTR pAppData, OMX_BUFFERHEADERTYPE* pBufferHeader);

  // OMXCore decoder callback routines.
  OMX_ERRORTYPE DecoderEventHandler(OMX_HANDLETYPE hComponent, OMX_PTR pAppData,
    OMX_EVENTTYPE eEvent, OMX_U32 nData1, OMX_U32 nData2, OMX_PTR pEventData);
  OMX_ERRORTYPE DecoderEmptyBufferDone(
    OMX_HANDLETYPE hComponent, OMX_PTR pAppData, OMX_BUFFERHEADERTYPE* pBuffer);
  OMX_ERRORTYPE DecoderFillBufferDone(
    OMX_HANDLETYPE hComponent, OMX_PTR pAppData, OMX_BUFFERHEADERTYPE* pBuffer);

  OMX_ERRORTYPE EmptyThisBuffer(OMX_BUFFERHEADERTYPE *omx_buffer);
  OMX_ERRORTYPE FillThisBuffer(OMX_BUFFERHEADERTYPE *omx_buffer);

  unsigned int GetInputBufferSize();
  unsigned int GetOutputBufferSize();

  unsigned int GetInputBufferSpace();
  unsigned int GetOutputBufferSpace();

  void FlushInput();
  void FlushOutput();

  OMX_BUFFERHEADERTYPE *GetInputBuffer();
  OMX_BUFFERHEADERTYPE *GetOutputBuffer();

  OMX_ERRORTYPE AllocInputBuffers(void);
  OMX_ERRORTYPE AllocOutputBuffers(void);

  OMX_ERRORTYPE FreeInputBuffers(bool wait);
  OMX_ERRORTYPE FreeOutputBuffers(bool wait);

private:
  OMX_HANDLETYPE m_handle;
  unsigned int   m_input_port;
  unsigned int   m_output_port;
  CStdString     m_componentName;
  pthread_mutex_t   m_omx_event_mutex;
  std::vector<omx_event> m_omx_events;

  OMX_CALLBACKTYPE  m_callbacks;

  OMX_BUFFERHEADERTYPE *m_omx_input_buffer;
  OMX_BUFFERHEADERTYPE *m_omx_output_buffer;

  // OMXCore input buffers (demuxer packets)
  pthread_mutex_t   m_omx_input_mutex;
  std::queue<OMX_BUFFERHEADERTYPE*> m_omx_input_avaliable;
  std::vector<OMX_BUFFERHEADERTYPE*> m_omx_input_buffers;
  unsigned int  m_input_alignment;
  unsigned int  m_input_buffer_size;
  unsigned int  m_input_buffer_count;

  // OMXCore output buffers (video frames)
  pthread_mutex_t   m_omx_output_mutex;
  std::queue<OMX_BUFFERHEADERTYPE*> m_omx_output_avaliable;
  std::vector<OMX_BUFFERHEADERTYPE*> m_omx_output_buffers;
  unsigned int  m_output_alignment;
  unsigned int  m_output_buffer_size;
  unsigned int  m_output_buffer_count;

  bool          m_exit;
  DllOMX        *m_DllOMX;
  bool          m_DllOMXOpen;
};

class COMXCore
{
public:
  COMXCore();
  ~COMXCore();

  // initialize OMXCore and get decoder component
  bool Initialize();
  void Deinitialize();

protected:
  bool              m_is_open;
  bool              m_Initialized;
  DllOMX            *m_DllOMX;
};

#endif

