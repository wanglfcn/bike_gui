#include <stdio.h>
#include <Arduino.h>
#include "OneButton.h"
#include "ui/ui.h"

int turnLeftStatus = 0;
int turnRightStatus = 0;
int lightStatus = 0;
int alarmStatus = 0;

unsigned long lightLastUpdateTs = 0;

#define ALARM_BTN_PIN 4
#define LEFT_BTN_PIN 5
#define RIGHT_BTN_PIN 7
#define LIGHT_BTN_PIN 6

#define LEFT_CTL_PIN 36
#define RIGHT_CTL_PIN 37
#define LIGHT_CTL_PIN 35

OneButton turnLeftBtn;
OneButton turnRightBtn;
OneButton alarmBtn;
OneButton lightBtn;

unsigned long btnLastUpdateTs = 0;

void triggerTurnLeft()
{
	if (alarmStatus != 0) {
		digitalWrite(LEFT_CTL_PIN, LOW);
		digitalWrite(RIGHT_CTL_PIN, LOW);
		alarmStatus = 0;
	}
	if (turnRightStatus != 0)
	{
		digitalWrite(RIGHT_CTL_PIN, LOW);
		turnRightStatus = 0;
	}

	if (turnLeftStatus == 0)
	{
		digitalWrite(LEFT_CTL_PIN, HIGH);
		turnLeftStatus = 1;
		lightLastUpdateTs = millis();
	}
	else
	{
		digitalWrite(LEFT_CTL_PIN, LOW);
		turnLeftStatus = 0;
	}
}

void triggerTurnRight()
{
	if (alarmStatus != 0) {
		digitalWrite(LEFT_CTL_PIN, LOW);
		digitalWrite(RIGHT_CTL_PIN, LOW);
		alarmStatus = 0;
	}
	if (turnLeftStatus != 0) {
		digitalWrite(LEFT_CTL_PIN, LOW);
		turnLeftStatus = 0;
	}

	if (turnRightStatus == 0)
	{
		digitalWrite(RIGHT_CTL_PIN, HIGH);
		turnRightStatus = 1;
		lightLastUpdateTs = millis();
	}
	else
	{
		digitalWrite(RIGHT_CTL_PIN, LOW);
		turnRightStatus = 0;
	}
}

void triggerAlarm() {
	if (turnLeftStatus != 0) {
		digitalWrite(LEFT_CTL_PIN, LOW);
		turnLeftStatus = 0;
	}

	if (turnRightStatus != 0) {
		digitalWrite(RIGHT_CTL_PIN, LOW);
		turnRightStatus = 0;
	}

	if (alarmStatus == 0) {
		digitalWrite(LEFT_CTL_PIN, HIGH);
		digitalWrite(RIGHT_CTL_PIN, HIGH);
		alarmStatus = 1;
		lightLastUpdateTs = millis();
	} else {
		digitalWrite(LEFT_CTL_PIN, LOW);
		digitalWrite(RIGHT_CTL_PIN, LOW);
		alarmStatus = 0;
	}
}

void triggerLight()
{
	if (lightStatus == 0)
	{
		digitalWrite(LIGHT_CTL_PIN, HIGH);
		lightStatus = 1;
        lv_obj_clear_flag(ui_lightIcon, LV_OBJ_FLAG_HIDDEN);
    }
	else
	{
		digitalWrite(LIGHT_CTL_PIN, LOW);
		lightStatus = 0;
        lv_obj_add_flag(ui_lightIcon, LV_OBJ_FLAG_HIDDEN);
	}
}

void initBtn()
{
	// setup OneButton
    pinMode(LEFT_BTN_PIN, INPUT_PULLUP);
    pinMode(RIGHT_BTN_PIN, INPUT_PULLUP);
    pinMode(LIGHT_BTN_PIN, INPUT_PULLUP);
    pinMode(ALARM_BTN_PIN, INPUT_PULLUP);


	turnLeftBtn.setup(LEFT_BTN_PIN, INPUT_PULLUP, true);
	turnLeftBtn.attachClick(triggerTurnLeft);

	pinMode(LEFT_CTL_PIN, OUTPUT);
	digitalWrite(LEFT_CTL_PIN, LOW);

	turnRightBtn.setup(RIGHT_BTN_PIN, INPUT_PULLUP, true);
	turnRightBtn.attachClick(triggerTurnRight);
	pinMode(RIGHT_CTL_PIN, OUTPUT);
	digitalWrite(RIGHT_CTL_PIN, LOW);

	alarmBtn.setup(ALARM_BTN_PIN, INPUT_PULLUP, true);
	alarmBtn.attachClick(triggerAlarm);

	lightBtn.setup(LIGHT_BTN_PIN, INPUT_PULLUP, true);
	lightBtn.attachClick(triggerLight);
	pinMode(LIGHT_CTL_PIN, OUTPUT);
	digitalWrite(LIGHT_CTL_PIN, LOW);

    lv_obj_add_flag(ui_lightIcon, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui_leftArr, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui_rightArr, LV_OBJ_FLAG_HIDDEN);
}

void updateLightInScreen() {
	int freq = 600;
	if (turnLeftStatus != 0 || turnRightStatus != 0 || alarmStatus != 0) {
		long diff = millis() - lightLastUpdateTs;
		int isShow = (diff / freq) % 2;
		if (isShow == 0) {
			if (alarmStatus != 0) {
				// set show left
                lv_obj_clear_flag(ui_leftArr, LV_OBJ_FLAG_HIDDEN);
                lv_obj_clear_flag(ui_rightArr, LV_OBJ_FLAG_HIDDEN);
            } else if (turnLeftStatus != 0) {
				// set show left
                lv_obj_clear_flag(ui_leftArr, LV_OBJ_FLAG_HIDDEN);
			} else {
				// set show right
                lv_obj_clear_flag(ui_rightArr, LV_OBJ_FLAG_HIDDEN);
			}

		} else {
				// set hide left
                lv_obj_add_flag(ui_leftArr, LV_OBJ_FLAG_HIDDEN);
				// set hide right
                lv_obj_add_flag(ui_rightArr, LV_OBJ_FLAG_HIDDEN);
		}
    }
    else
    {
        // set hide left
        lv_obj_add_flag(ui_leftArr, LV_OBJ_FLAG_HIDDEN);
        // set hide right
        lv_obj_add_flag(ui_rightArr, LV_OBJ_FLAG_HIDDEN);
    }
}

void updateStatus(unsigned long now)
{
    if ((now - btnLastUpdateTs) < 50)
    {
        return;
    }
    btnLastUpdateTs = now;

    turnLeftBtn.tick();
    turnRightBtn.tick();
    alarmBtn.tick();
    lightBtn.tick();
    updateLightInScreen();
}