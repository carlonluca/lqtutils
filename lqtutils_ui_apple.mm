/**
 * MIT License
 *
 * Copyright (c) 2023 Luca Carlon
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 **/

#include "lqtutils_ui.h"

#import <UserNotifications/UserNotifications.h>
#import <Foundation/Foundation.h>

@interface NotificationDelegate : NSObject<UNUserNotificationCenterDelegate>
@end

@implementation NotificationDelegate

-(void)userNotificationCenter:(UNUserNotificationCenter*)center
      willPresentNotification:(UNNotification*)notification
        withCompletionHandler:(void (^)(UNNotificationPresentationOptions options))completionHandler
{
    Q_UNUSED(center)
    completionHandler(UNNotificationPresentationOptionAlert);
}

 -(void)userNotificationCenter:(UNUserNotificationCenter*)center
didReceiveNotificationResponse:(UNNotificationResponse*)response
         withCompletionHandler:(void(^)())completionHandler
{
    Q_UNUSED(center)
    Q_UNUSED(response)
    completionHandler();
}

@end

namespace lqt {

void AppleSystemNotification::send()
{
    UNMutableNotificationContent* content = [[[UNMutableNotificationContent alloc] init] autorelease];
    content.title = m_title.toNSString();
    content.body = m_message.toNSString();

    UNTimeIntervalNotificationTrigger* trigger = [UNTimeIntervalNotificationTrigger triggerWithTimeInterval:1 repeats:NO];
    UNNotificationRequest* request = [UNNotificationRequest requestWithIdentifier:@"LocalNotification" content:content trigger:trigger];

    UNUserNotificationCenter* center = [UNUserNotificationCenter currentNotificationCenter];
    if (!center.delegate)
      center.delegate = [[NotificationDelegate alloc] init];
    [center addNotificationRequest:request withCompletionHandler:^(NSError * _Nullable error) {
        if (error) {
            qDebug() << "Error adding notification request:" << QString::fromNSString(error.localizedDescription);
        }
    }];
}

void AppleSystemNotification::requestPermission(std::function<void(bool)> callback)
{
    UNUserNotificationCenter* center = [UNUserNotificationCenter currentNotificationCenter];
    [center requestAuthorizationWithOptions:(UNAuthorizationOptionBadge | UNAuthorizationOptionAlert | UNAuthorizationOptionSound)
                          completionHandler:^(BOOL granted, NSError * _Nullable error) {
        if (error) {
            qWarning() << "Error occurred requesting authorization:"
                       << QString::fromNSString([error localizedDescription])
                       << QString::fromNSString([error localizedFailureReason]);
            callback(false);
            return;
        }

        callback(true);
    }];
}

} // namespace
