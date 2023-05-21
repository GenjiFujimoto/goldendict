#include "ankiconnector.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include "qt4x5.hh"

QString markTargetWord(QString const& sentence, QString const& word)
{
  QString result = sentence;
  return result.replace(word, "<b>" + word + "</b>", Qt::CaseInsensitive);
}

AnkiConnector::AnkiConnector( QObject * parent, Config::Class const & _cfg ) : QObject{ parent }, cfg( _cfg )
{
  mgr = new QNetworkAccessManager( this );
  connect( mgr, &QNetworkAccessManager::finished, this, &AnkiConnector::finishedSlot );
}

void AnkiConnector::sendToAnki( QString const & word, QString text, QString const & sentence )
{
  // Anki doesn't understand the newline character, so it should be replaced.
  text = text.replace("\n", "<br>");

  QString const postTemplate = R"anki({
      "action": "addNote",
      "version": 6,
      "params": {
          "note": {
              "deckName": "%1",
              "modelName": "%2",
              "fields": %3,
              "options": {
                  "allowDuplicate": true
              },
              "tags": []
          }
      }
  })anki";

  QJsonObject fields;
  fields.insert( cfg.preferences.ankiConnectServer.word, word );
  fields.insert( cfg.preferences.ankiConnectServer.text, text );
  if (!cfg.preferences.ankiConnectServer.sentence.isEmpty()) {
    QString sentence_changed = markTargetWord(sentence, word);
    fields.insert( cfg.preferences.ankiConnectServer.sentence, sentence_changed );
  }

  QString postData = postTemplate.arg( cfg.preferences.ankiConnectServer.deck,
                                       cfg.preferences.ankiConnectServer.model,
                                       Qt4x5::json2String( fields ) );

  postToAnki( postData );
}

void AnkiConnector::postToAnki( QString const & postData )
{
  QUrl url;
  url.setScheme( "http" );
  url.setHost( cfg.preferences.ankiConnectServer.host );
  url.setPort( cfg.preferences.ankiConnectServer.port );
  QNetworkRequest request( url );
  request.setTransferTimeout( 3000 );
  request.setHeader( QNetworkRequest::ContentTypeHeader, "application/json" );
  QNetworkReply *reply = mgr->post( request, postData.toUtf8() );
  connect( reply,
           &QNetworkReply::errorOccurred,
           this,
           [ this ]( QNetworkReply::NetworkError e )
           {
             qWarning() << e;
             emit this->errorText( tr( "anki: post to anki failed" ) );
           } );
}

void AnkiConnector::finishedSlot( QNetworkReply * reply )
{
  if( reply->error() == QNetworkReply::NoError )
  {
    QByteArray bytes   = reply->readAll();
    QJsonDocument json = QJsonDocument::fromJson( bytes );
    auto obj           = json.object();
    if( obj.size() != 2 || !obj.contains( "error" ) || !obj.contains( "result" ) ||
        obj[ "result" ].toString().isEmpty() )
    {
      emit errorText( QObject::tr( "anki: post to anki failed" ) );
    }
    QString result = obj[ "result" ].toString();

    qDebug() << "anki result:" << result;

    emit errorText( tr( "anki: post to anki success" ) );
  }
  else
  {
    qDebug() << "anki connect error" << reply->errorString();
    emit errorText( "anki:" + reply->errorString() );
  }

  reply->deleteLater();
}
