#ifndef CLASSSKILLS_H
#define CLASSSKILLS_H


#include <QString>
#include <QMap>
#include <QDateTime>
#include <QDataStream>

#include <classprojekt.h>

class ClassSkills
{
    //Q_ENUMS(CriteriaTypes)

public:
    ClassSkills();

    // Evaluation criteria
    enum class CriteriaTypes { projectNode, identifierNode };


    int getNr() const;
    void setNr(int nr);

    QString name() const;
    void setName(const QString &name);

    QDate date() const;
    void setDate(const QDate &date);

    QString identifier() const;
    void setIdentifier(const QString &identifier);

    QMap<QString, ClassProjekt> getProjektMap() const;
    void setProjektMap(const QMap<QString, ClassProjekt> &value);

    QDateTime getCreatedDate() const;
    void setCreatedDate(const QDateTime &createdDate);

    int getWert() const;
    void setWert(int wert);

    QString getKey();
    bool isValid();

    int maxPoints();
    int points();

    QMap<QString, double> getIdentMap() const;
    void setIdentMap(const QMap<QString, double> &iMap);
    void setIdentFactor(const QString &key, double value);
    double getIdentFactor(const QString &key);

    bool isEvaluated();

    /// !brief Evaluation criteria
    void setEvaluationType(const CriteriaTypes &type);
    void setEvaluationType(int index);
    int getEvaluationIndex() const;
    QString getEvaluationText(int index) const;

    static QStringList supportedCriteria(){
        QStringList list;
        list << "Nach Projekten" << "Nach Kennung (Fragen)";
        return list;
    }

    bool containsProject(const QString &proKey);
    void insertProjekt( ClassProjekt pro);
    bool removeProject( const QString &proKey );
    QStringList projectKeyList();

private:

    int m_nr;
    QString m_name;
    QString m_identifier;
    QDate m_date;
    QDateTime m_createdDate;
    int m_wert;
    CriteriaTypes m_evaluationType;

    QMap<QString, ClassProjekt> projektMap;
    QMap<QString, double> identMap;


};

QDataStream &operator<<(QDataStream &out, const ClassSkills &dat);
QDataStream &operator>>(QDataStream &in, ClassSkills &dat);

#endif // CLASSSKILLS_H
