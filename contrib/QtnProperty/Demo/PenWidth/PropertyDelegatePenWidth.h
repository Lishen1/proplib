/*
   Copyright (c) 2012-2016 Alex Zhondin <lexxmark.dev@gmail.com>

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#ifndef PROPERTY_DELEGATE_PEN_WIDTH_H
#define PROPERTY_DELEGATE_PEN_WIDTH_H

#include "Delegates/Utils/PropertyDelegateMisc.h"

class QtnPropertyPenWidthBase;

class QtnPropertyDelegatePenWidth: public QtnPropertyDelegateTyped<QtnPropertyPenWidthBase>
{
    Q_DISABLE_COPY(QtnPropertyDelegatePenWidth)

public:
    QtnPropertyDelegatePenWidth(QtnPropertyPenWidthBase& owner)
        : QtnPropertyDelegateTyped<QtnPropertyPenWidthBase>(owner)
    {
    }

protected:
    bool propertyValueToStrImpl(QString& strValue) const override;
    void drawValueImpl(QStylePainter& painter, const QRect& rect, const QStyle::State& state, bool* needTooltip = nullptr) const override;
    QWidget* createValueEditorImpl(QWidget* parent, const QRect& rect, QtnInplaceInfo* inplaceInfo = nullptr) override;
};

#endif // PROPERTY_DELEGATE_PEN_WIDTH_H
