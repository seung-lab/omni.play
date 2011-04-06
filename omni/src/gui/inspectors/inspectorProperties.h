#ifndef INSPECTOR_PROPERTIES_H
#define INSPECTOR_PROPERTIES_H

#include "events/details/omUserInterfaceEvent.h"
#include "events/details/omSegmentEvent.h"

#include <QtGui>
#include <QWidget>

class OmViewGroupState;

class InspectorProperties : public QDialog,
                            public OmUserInterfaceEventListener,
                            public OmSegmentEventListener {
Q_OBJECT

public:
    InspectorProperties(QWidget *parent, OmViewGroupState * vgs);

    virtual ~InspectorProperties();

    void SetOrReplaceWidget(QWidget *incomingWidget, const QString& title);

    OmViewGroupState* getViewGroupState(){
        return vgs_;
    }

public Q_SLOTS:
    void closeDialog();

private:
    OmViewGroupState *const vgs_;

    boost::scoped_ptr<QWidget> widget_;

    QVBoxLayout* mainLayout_;

    void UpdateSegmentPropWidgetEvent(OmUserInterfaceEvent*);

    void SegmentGUIlistEvent(OmSegmentEvent*)
    {}

    void SegmentSelectedEvent(OmSegmentEvent*)
    {}

    void SegmentModificationEvent(OmSegmentEvent* event);
};

#endif
