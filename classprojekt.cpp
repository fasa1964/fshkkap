#include "classprojekt.h"
#include <QtMath>

ClassProjekt::ClassProjekt()
{
    m_name = "";
    m_identifier = "";
    m_nr = 0;
    m_countQuestion = 0;
    m_maxPoints = 0;
    m_points = 0;
    m_percent = 0.0;
    m_note = 0;
    m_document = "";
    m_createTime = "";
    m_lockSubject = false;
    m_evaluated = false;
    m_factor = 1.0;
    m_duration = 0;
}

QString ClassProjekt::name() const
{
    return m_name;
}

void ClassProjekt::setName(const QString &name)
{
    m_name = name;
}

QString ClassProjekt::identifier() const
{
    return m_identifier;
}

void ClassProjekt::setIdentifier(const QString &identifier)
{
    m_identifier = identifier;
}

int ClassProjekt::nr() const
{
    return m_nr;
}

void ClassProjekt::setNr(int nr)
{
    m_nr = nr;
}

int ClassProjekt::countQuestion() const
{
    return m_countQuestion;
}

void ClassProjekt::setCountQuestion(int countQuestion)
{
    m_countQuestion = countQuestion;
}

int ClassProjekt::maxPoints() const
{
    return m_maxPoints;
}

void ClassProjekt::setMaxPoints(int maxPoints)
{
    m_maxPoints = maxPoints;
}

int ClassProjekt::points() const
{
    return m_points;
}

void ClassProjekt::setPoints(int points)
{
    m_points = points;
}

// Test another way to calculate the percent
double ClassProjekt::percent() const
{
    int p = 0;
    foreach (ClassFrage question, questionMap().values()) {
        p += question.points();
    }

    double percent =  p * 100.0 / maxPoints();

    return percent;
}

void ClassProjekt::setPercent(double percent)
{
    m_percent = percent;
}

int ClassProjekt::note() const
{
    return m_note;
}

void ClassProjekt::setNote(int note)
{
    m_note = note;
}

QMap<int, ClassFrage> ClassProjekt::questionMap() const
{
    return m_questionMap;
}

void ClassProjekt::setQuestionMap(const QMap<int, ClassFrage> &questionMap)
{
    m_questionMap = questionMap;
}


QString ClassProjekt::document() const
{
    return m_document;
}

void ClassProjekt::setDocument(const QString &document)
{
    m_document = document;
}

QDateTime ClassProjekt::dateTime() const
{
    return m_dateTime;
}

void ClassProjekt::setDateTime(const QDateTime &dateTime)
{
    m_dateTime = dateTime;
}

QString ClassProjekt::createTime() const
{
    return m_createTime;
}

void ClassProjekt::setCreateTime(const QString &createTime)
{
    m_createTime = createTime;
}

bool ClassProjekt::lockSubject() const
{
    return m_lockSubject;
}

void ClassProjekt::setLockSubject(bool lockSubject)
{
    m_lockSubject = lockSubject;
}

QString ClassProjekt::getKey()
{
    QString key;
    key = name()+"."+identifier();
    return key;
}

bool ClassProjekt::isValid()
{
    if(name().isEmpty() || identifier().isEmpty())
        return false;
    else
        return true;
}

void ClassProjekt::clearValues()
{
    setPoints(0);
    setPercent(0.0);
    setEvaluated(false);
    QMap<int, ClassFrage> qMap = questionMap();
    foreach (ClassFrage frg, qMap.values()) {
        frg.setPoints(0);
    }
    setQuestionMap(qMap);
}

bool ClassProjekt::getEvaluated() const
{
    return m_evaluated;
}

void ClassProjekt::setEvaluated(bool evaluated)
{
    m_evaluated = evaluated;
}

void ClassProjekt::setQuestionPoints(int nr, int points)
{
    ClassFrage frg = m_questionMap.value(nr);
    frg.setPoints(points);
    m_questionMap.insert(nr, frg);
}


double ClassProjekt::getFactor() const
{
    return m_factor;
}

void ClassProjekt::setFactor(double factor)
{
    m_factor = factor;
}

int ClassProjekt::getDuration() const
{
    return m_duration;
}

void ClassProjekt::setDuration(int duration)
{
    m_duration = duration;
}

QStringList ClassProjekt::identifierList()
{
    QStringList identList;
    QMapIterator<int, ClassFrage> it(questionMap());
    while (it.hasNext()) {
        it.next();
        ClassFrage question = it.value();
        if(!question.identifier().isEmpty()){
            QString ki = question.identifier();
            if(!identList.contains(ki))
                identList << ki;
        }
    }

    return identList;
}

double ClassProjekt::getPercentIdent(const QString &id)
{
    int maxPoint = 0;
    int points = 0;

    QMapIterator<int, ClassFrage> it(questionMap());
    while (it.hasNext()) {
        it.next();
        ClassFrage q = it.value();
        if(q.identifier() == id){
            maxPoint += q.maxPoints();
            points += q.points();
        }
    }

    return points * 100.0 / maxPoint;
}

int ClassProjekt::getPointsIdent(const QString &id)
{
    int points = 0;

    QMapIterator<int, ClassFrage> it(questionMap());
    while (it.hasNext()) {
        it.next();
        ClassFrage q = it.value();
        if(q.identifier() == id)
            points += q.points();
    }

    return points;
}

int ClassProjekt::getMaxPointsIdent(const QString &id)
{
    int maxPoint = 0;

    QMapIterator<int, ClassFrage> it(questionMap());
    while (it.hasNext()) {
        it.next();
        ClassFrage q = it.value();
        if(q.identifier() == id)
            maxPoint += q.maxPoints();
    }

    return maxPoint;
}

QString ClassProjekt::getDescrition() const
{
    return m_descrition;
}

void ClassProjekt::setDescrition(const QString &descrition)
{
    m_descrition = descrition;
}

QDataStream &operator<<(QDataStream &out, const ClassProjekt &dat)
{
    out << dat.nr() << dat.name() << dat.identifier() << dat.getDescrition() << dat.countQuestion() <<
           dat.maxPoints() << dat.points() << dat.percent() << dat.note() << dat.questionMap() <<
           dat.document() << dat.dateTime() << dat.createTime() << dat.lockSubject() <<
           dat.getEvaluated() << dat.getFactor() << dat.getDuration();
    return out;
}

QDataStream &operator>>(QDataStream &in, ClassProjekt &dat)
{
    QString name;
    QString identifier;
    QString desc;
    int nr;
    int countQuestion;
    int maxPoints;
    int points;
    double percent;
    int note;
    QMap<int, ClassFrage> frgMap;
    QString doc;
    QString dts;
    QDateTime dt;
    bool sperr;
    bool evaluated;
    double fac;
    int min;

    in >> nr >> name >> identifier >> desc >> countQuestion >> maxPoints >> points
       >> percent >> note >> frgMap >> doc >> dt >> dts >> sperr >> evaluated
       >> fac >> min;


    dat.setNr(nr);
    dat.setName(name);
    dat.setIdentifier(identifier);
    dat.setDescrition(desc);
    dat.setCountQuestion(countQuestion);
    dat.setMaxPoints(maxPoints);
    dat.setPoints(points);
    dat.setPercent(percent);
    dat.setNote(note);
    dat.setQuestionMap(frgMap);
    dat.setDocument(doc);
    dat.setDateTime(dt);
    dat.setCreateTime(dts);
    dat.setLockSubject(sperr);
    dat.setEvaluated(evaluated);
    dat.setFactor(fac);
    dat.setDuration(min);

    return in;
}



