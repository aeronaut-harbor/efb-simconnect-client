#include <Windows.h>

#include <iostream>

#pragma warning(push)
#pragma warning(disable : 4245)
#include "SimConnect SDK/include/SimConnect.h"
#pragma warning(pop)

enum DataDefinition {
  kPlanePositionDataDefinition,
};

enum Request {
  kUserPlanePositionRequest,
};

struct PlanePosition {
  double latitude;
  double longitude;
};

struct DispatchContext {
  bool quit_received = false;
};

void CALLBACK Dispatch(SIMCONNECT_RECV* data, DWORD /*data_size*/,
                       void* context) {
  auto dispatch_context = static_cast<DispatchContext*>(context);

  switch (data->dwID) {
    case SIMCONNECT_RECV_ID_SIMOBJECT_DATA: {
      auto simobject_data =
          reinterpret_cast<SIMCONNECT_RECV_SIMOBJECT_DATA*>(data);
      if (simobject_data->dwRequestID != Request::kUserPlanePositionRequest) {
        std::cout << "Unexpected request ID: " << simobject_data->dwRequestID
                  << "." << std::endl;
        break;
      }
      auto plane_position =
          reinterpret_cast<PlanePosition*>(&simobject_data->dwData);
      std::cout << "Plane position: " << plane_position->latitude << ", "
                << plane_position->longitude << "." << std::endl;
      break;
    }

    case SIMCONNECT_RECV_ID_QUIT:
      std::cout << "SIMCONNECT_RECV_ID_QUIT received." << std::endl;
      dispatch_context->quit_received = true;
      break;
  }
}

int main(int /*argc*/, char* /*argv*/[]) {
  HANDLE simconnect_handle;
  if (SimConnect_Open(&simconnect_handle, "Aeronaut Harbor EFB", nullptr, 0,
                      nullptr, 0) != S_OK) {
    std::cout << "SimConnect_Open() failed." << std::endl;
    return 1;
  }

  SimConnect_AddToDataDefinition(simconnect_handle,
                                 kPlanePositionDataDefinition, "PLANE LATITUDE",
                                 "degrees");
  SimConnect_AddToDataDefinition(simconnect_handle,
                                 kPlanePositionDataDefinition,
                                 "PLANE LONGITUDE", "degrees");
  SimConnect_RequestDataOnSimObject(
      simconnect_handle, kUserPlanePositionRequest,
      kPlanePositionDataDefinition, SIMCONNECT_OBJECT_ID_USER,
      SIMCONNECT_PERIOD_SECOND);

  DispatchContext dispatch_context;
  while (!dispatch_context.quit_received) {
    if (SimConnect_CallDispatch(simconnect_handle, &Dispatch,
                                &dispatch_context) != S_OK) {
      std::cout << "SimConnect_CallDispatch() failed." << std::endl;
    }
  }

  if (SimConnect_Close(simconnect_handle) != S_OK) {
    std::cout << "SimConnect_Close() failed." << std::endl;
    return 1;
  }

  return 0;
}
