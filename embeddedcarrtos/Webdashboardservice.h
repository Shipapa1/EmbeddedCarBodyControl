#ifndef WEB_DASHBOARD_SERVICE_H
#define WEB_DASHBOARD_SERVICE_H

// ---------------------------------------------------------------------------
// WebDashboardService.h
// Joins the shared car WiFi network (hosted by the motor-control ESP32's
// access point) and serves a live BCM status dashboard over HTTP.
//
// Call WebDashboardService_Init() once from setup(), then spawn
// WebDashboardService as its own FreeRTOS task, same pattern as every
// other service in this project.
// ---------------------------------------------------------------------------

void WebDashboardService_Init();
void WebDashboardService(void *parameter);

#endif // WEB_DASHBOARD_SERVICE_H