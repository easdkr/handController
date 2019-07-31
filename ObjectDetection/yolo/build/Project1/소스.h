#pragma once

void listen_client();

void device_Id_Int_to_parse(int iObjectId, char * cObjectId);

void gesture_event_handler();

int Hand_in_Range(std::vector<bbox_t> result_vec);

void communicate_hand_tracker();
