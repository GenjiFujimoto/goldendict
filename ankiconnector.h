#ifndef ANKICONNECTOR_H
#define ANKICONNECTOR_H

#include "config.hh"

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QObject>

class AnkiConnector : public QObject
{
  Q_OBJECT
public:
  explicit AnkiConnector( QObject * parent, Config::Class const & cfg );

  void sendToAnki( QString const & word, QString text, QString const & sentence );

private:
  QNetworkAccessManager * mgr;
  Config::Class const & cfg;
  void postToAnki( QString const & postData );
public :
signals:
  void errorText( QString const & );
private slots:
  void finishedSlot(QNetworkReply * reply);
};

#endif // ANKICONNECTOR_H
