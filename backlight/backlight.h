#ifndef _BACKLIGHT_H_
#define _BACKLIGHT_H_

#define	BACKLIGHT_FREQUENCY_HZ	1000
#define BACKLIGHT_DUTY_CYCLE	0

void BackLightInit (void);
int BackLightLewel (int Lewel_percentage);
int BackLightFrequency (int BackLightFrequency_Hz);

#endif
