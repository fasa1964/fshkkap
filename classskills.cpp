#include "classskills.h"

ClassSkills::ClassSkills()
{
    m_nr = 0;
    m_name = "";
    m_identifier = "";
    m_wert = 0;
    m_date = QDate();
    m_evaluationType = CriteriaTypes::projectNode;

}


QString ClassSkills::name() const
{
    return m_name;
}

void ClassSkills::setName(const QString &name)
{
    m_name = name;
}

QDate ClassSkills::date() const
{
    return m_date;
}

void ClassSkills::setDate(const QDate &date)
{
    m_date = date;
}

QString ClassSkills::identifier() const
{
    return m_identifier;
}

void ClassSkills::setIdentifier(const QString &identifier)
{
    m_identifier = identifier;
}

QMap<QString, ClassProjekt> ClassSkills::getProjektMap() const
{
    return projektMap;
}

void ClassSkills::setProjektMap(const QMap<QString, ClassProjekt> &value)
{
    projektMap = value;
}


QDateTime ClassSkills::getCreatedDate() const
{
    return m_createdDate;
}

void ClassSkills::setCreatedDate(const QDateTime &createdDate)
{
    m_createdDate = createdDate;
}

int ClassSkills::getWert() const
{
    return m_wert;
}

void ClassSkills::setWert(int wert)
{
    m_wert = wert;
}

QString ClassSkills::getKey()
{
    QString key = name()+"."+identifier();
    return key;
}

bool ClassSkills::isValid()
{
    if(name().isEmpty() || identifier().isEmpty())
        return false;
    else
        return true;
}

int ClassSkills::maxPoints()
{
    int maxpunkte = 0;
    QMapIterator<QString, ClassProjekt> it(projektMap);
    while (it.hasNext()){
         it.next();
         maxpunkte = maxpunkte + it.value().maxPoints();
    }

    return maxpunkte;
}

int ClassSkills::points()
{
    int punkte = 0;
    QMapIterator<QString, ClassProjekt> it(projektMap);
    while (it.hasNext()){
         it.next();
         punkte = punkte + it.value().points();
    }

    return punkte;
}

bool ClassSkills::isEvaluated()
{
    bool status = false;

    QMapIterator<QString, ClassProjekt> it(projektMap);
    while (it.hasNext()) {
        it.next();
        if(it.value().getEvaluated()){
            status = true;
            break;
        }
    }

    return status;
}

void ClassSkills::setEvaluationType(const CriteriaTypes &type)
{
    m_evaluationType = type;
}

void ClassSkills::setEvaluationType(int index)
{
    if(index == 0)
        m_evaluationType = CriteriaTypes::projectNode;

    if(index == 1)
        m_evaluationType = CriteriaTypes::identifierNode;

}

int ClassSkills::getEvaluationIndex() const
{
    int index = -1;

    if(m_evaluationType == CriteriaTypes::projectNode)
        index = 0;

    if(m_evaluationType == CriteriaTypes::identifierNode)
        index = 1;

    return index;
}

QString ClassSkills::getEvaluationText(int index) const
{
    QString text;

    if(index == 0)
        text = "Nach Projekten";

    if(index == 1)
        text = "Nach Kennung (Fragen)";

    return text;
}

int ClassSkills::getNr() const
{
    return m_nr;
}

void ClassSkills::setNr(int nr)
{
    m_nr = nr;
}

bool ClassSkills::containsProject(const QString &proKey)
{
    return projektMap.keys().contains(proKey);
}

void ClassSkills::insertProjekt(ClassProjekt pro)
{
    projektMap.insert(pro.getKey(), pro);
}

bool ClassSkills::removeProject(const QString &proKey)
{
    bool status = false;
    if(projektMap.remove(proKey) == 1)
        status = true;
    return status;
}

QStringList ClassSkills::projectKeyList()
{
    QStringList keyList;
    foreach (ClassProjekt p, projektMap.values()) {
        keyList << p.getKey();
    }

    return keyList;
}


QDataStream &operator<<(QDataStream &out, const ClassSkills &dat)
{
    out << dat.getNr() << dat.name() << dat.identifier() << dat.date() << dat.getWert() <<
           dat.getCreatedDate() << dat.getProjektMap() << dat.getEvaluationIndex();
    return out;
}

QDataStream &operator>>(QDataStream &in, ClassSkills &dat)
{
    int nr;
    QString name;
    QString identifier;
    QDate date;
    QDateTime createdDate;
    int wert;
    QMap<QString, ClassProjekt> pMap;
    int evalType;

    in >> nr >> name >> identifier >> date >> wert >> createdDate >> pMap >> evalType;

    dat.setNr( nr );
    dat.setName( name );
    dat.setIdentifier( identifier );
    dat.setWert( wert );
    dat.setDate( date );
    dat.setCreatedDate( createdDate );
    dat.setProjektMap( pMap );
    dat.setEvaluationType(evalType);

    return in;
}
