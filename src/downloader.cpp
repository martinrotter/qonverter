/*
	This file is part of Qonverter.
	
	Qonverter is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.
	
	Qonverter is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.
	
	You should have received a copy of the GNU General Public License
	along with Qonverter.  If not, see <http://www.gnu.org/licenses/>.
	
	Copyright 2012 - 2013 Martin Rotter
*/

#include <QEventLoop>
#include <QTimer>
#include <QNetworkReply>
#include <QDebug>
#include <QRegExp>

#include "downloader.h"
#include "defs.h"


QNetworkReply::NetworkError Downloader::downloadFile(const QString &url,
                                                     QByteArray &output,
                                                     int timeout) {
  // Original asynchronous behavior of QNetworkAccessManager
  // is replaced by synchronous behavior in order to make
  // process of downloading of a file easier to understand.
  QNetworkAccessManager manager;
  QEventLoop loop;
  QTimer timer;
  QNetworkRequest request;
  QNetworkReply *reply;

  // Set some information for HTTP request.
  request.setUrl(url);
  request.setRawHeader("User-Agent", APP_LOW_NAME);

  // Create necessary connections.
  QObject::connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
  QObject::connect(&manager, &QNetworkAccessManager::finished, &loop, &QEventLoop::quit);

  forever {
    timer.setSingleShot(true);
    reply = manager.get(request);
    reply->ignoreSslErrors();

    timer.start(timeout);
    loop.exec();

    if (timer.isActive() == true) {
      // Download is complete.
      timer.stop();
    }
    else {
      return QNetworkReply::TimeoutError;
    }

    if (reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl().isValid() == false) {
      break;
    }
    else {
      // QUrl indicates redirection, so redirect
      request.setUrl(reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl());
    }
  }

  // Store the answer.
  output = reply->readAll();

  QNetworkReply::NetworkError ret = reply->error();
  if (ret == QNetworkReply::NoError) {
    qDebug("File %s fetched successfully with code %d.",
           qPrintable(url),
           reply->error());
  }
  else {
    qWarning("File %s fetched with error: %s.",
             qPrintable(url),
             qPrintable(reply->errorString()));
  }

  // Free resources.
  reply->deleteLater();
  return ret;
}
