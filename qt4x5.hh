/* Thin wrappers for retaining compatibility for both Qt4.x and Qt5.x */

#ifndef QT4X5_HH
#define QT4X5_HH

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
# define IS_QT_5    0
#else
# define IS_QT_5    1
#endif

#include <QProcess>
#include <QString>
#include <QAtomicInt>
#include <QTextDocument>
#include <QJsonObject>
#include <QJsonDocument>

#include <QUrl>
#if IS_QT_5
#include <QUrlQuery>
#endif

namespace Qt4x5
{

#if QT_VERSION >= QT_VERSION_CHECK( 5, 14, 0 )
inline Qt::SplitBehaviorFlags keepEmptyParts()
{ return Qt::KeepEmptyParts; }
inline Qt::SplitBehaviorFlags skipEmptyParts()
{ return Qt::SkipEmptyParts; }
#else
inline QString::SplitBehavior keepEmptyParts()
{ return QString::KeepEmptyParts; }
inline QString::SplitBehavior skipEmptyParts()
{ return QString::SkipEmptyParts; }
#endif

inline Qt::MouseButton middleButton()
{
#if QT_VERSION >= QT_VERSION_CHECK( 4, 7, 0 )
  return Qt::MiddleButton;
#else
  return Qt::MidButton;
#endif
}

inline QString escape( QString const & plain )
{
#if IS_QT_5
  return plain.toHtmlEscaped();
#else
  return Qt::escape( plain );
#endif

}

inline QString json2String( const QJsonObject & json )
{
  return QString( QJsonDocument( json ).toJson( QJsonDocument::Compact ) );
}

namespace AtomicInt
{

inline int loadAcquire( QAtomicInt const & ref )
{
#if IS_QT_5
  return ref.loadAcquire();
#else
  return ( int )ref;
#endif
}

}

namespace Url
{

// This wrapper is created due to behavior change of the setPath() method
// See: https://bugreports.qt-project.org/browse/QTBUG-27728
//       https://codereview.qt-project.org/#change,38257
inline QString ensureLeadingSlash( const QString & path )
{
#if IS_QT_5
  QLatin1Char slash( '/' );
  if ( path.startsWith( slash ) )
    return path;
  return slash + path;
#else
  return path;
#endif
}

inline bool hasQueryItem( QUrl const & url, QString const & key )
{
#if IS_QT_5
  return QUrlQuery( url ).hasQueryItem( key );
#else
  return url.hasQueryItem( key );
#endif
}

inline QString queryItemValue( QUrl const & url, QString const & item )
{
#if IS_QT_5
  return QUrlQuery( url ).queryItemValue( item, QUrl::FullyDecoded );
#else
  return url.queryItemValue( item );
#endif
}

inline QByteArray encodedQueryItemValue( QUrl const & url, QString const & item )
{
#if IS_QT_5
  return QUrlQuery( url ).queryItemValue( item, QUrl::FullyEncoded ).toLatin1();
#else
  return url.encodedQueryItemValue( item.toLatin1() );
#endif
}

inline void addQueryItem( QUrl & url, QString const & key, QString const & value )
{
#if IS_QT_5
  QUrlQuery urlQuery( url );
  urlQuery.addQueryItem( key, value );
  url.setQuery( urlQuery );
#else
  url.addQueryItem( key, value );
#endif
}

inline void removeQueryItem( QUrl & url, QString const & key )
{
#if IS_QT_5
  QUrlQuery urlQuery( url );
  urlQuery.removeQueryItem( key );
  url.setQuery( urlQuery );
#else
  url.removeQueryItem( key );
#endif
}

inline QString fullPath( QUrl const & url )
{
#if IS_QT_5
  QString path = url.path( QUrl::FullyDecoded );
  if( url.hasQuery() )
  {
    QUrlQuery urlQuery( url );
    path += QString::fromLatin1( "?" ) + urlQuery.toString( QUrl::FullyDecoded );
  }
  return path;
#else
  return url.toString( QUrl::RemoveScheme | QUrl::RemoveAuthority | QUrl::RemoveFragment | QUrl::RemovePort );
#endif
}

inline void setQueryItems( QUrl & url, QList< QPair< QString, QString > > const & query )
{
#if IS_QT_5
  QUrlQuery urlQuery( url );
  urlQuery.setQueryItems( query );
  url.setQuery( urlQuery );
#else
  url.setQueryItems( query );
#endif
}

inline QString path( QUrl const & url )
{
#if IS_QT_5
  return url.path( QUrl::FullyDecoded );
#else
  return url.path();
#endif
}

inline void setFragment( QUrl & url, const QString & fragment )
{
#if IS_QT_5
  url.setFragment( fragment, QUrl::DecodedMode );
#else
  url.setFragment( fragment );
#endif
}

inline QString fragment( const QUrl & url )
{
#if IS_QT_5
  return url.fragment( QUrl::FullyDecoded );
#else
  return url.fragment();
#endif
}

}

namespace Dom
{

#if IS_QT_5
typedef int size_type;
#else
typedef uint size_type;
#endif

}

namespace Process
{

inline bool startDetached( QString const & command )
{
#if QT_VERSION >= QT_VERSION_CHECK( 5, 15, 0 )
  auto args = QProcess::splitCommand( command );
  if( args.empty() )
    return false;
  auto const program = args.takeFirst();
  return QProcess::startDetached( program, args );
#else
  return QProcess::startDetached( command );
#endif
}

}

}

#endif // QT4X5_HH
