cmake_minimum_required(VERSION 2.8)
project(QtnProperty)


find_package(Qt5 COMPONENTS Widgets Script)
message(STATUS ${CMAKE_CURRENT_BINARY_DIR})

set(CMAKE_AUTOMOC TRUE)

set(QtnPropertySource 
	"${CMAKE_CURRENT_SOURCE_DIR}/Core/PropertyBase.cpp" 
    "${CMAKE_CURRENT_SOURCE_DIR}/Core/Property.cpp" 
    "${CMAKE_CURRENT_SOURCE_DIR}/Core/PropertySet.cpp" 
    "${CMAKE_CURRENT_SOURCE_DIR}/Core/Enum.cpp" 
    "${CMAKE_CURRENT_SOURCE_DIR}/Core/QObjectPropertySet.cpp" 
    "${CMAKE_CURRENT_SOURCE_DIR}/Core/Core/PropertyBool.cpp" 
    "${CMAKE_CURRENT_SOURCE_DIR}/Core/Core/PropertyInt.cpp" 
    "${CMAKE_CURRENT_SOURCE_DIR}/Core/Core/PropertyUInt.cpp" 
    "${CMAKE_CURRENT_SOURCE_DIR}/Core/Core/PropertyFloat.cpp" 
    "${CMAKE_CURRENT_SOURCE_DIR}/Core/Core/PropertyDouble.cpp" 
    "${CMAKE_CURRENT_SOURCE_DIR}/Core/Core/PropertyQString.cpp" 
    "${CMAKE_CURRENT_SOURCE_DIR}/Core/Core/PropertyQRect.cpp" 
    "${CMAKE_CURRENT_SOURCE_DIR}/Core/Core/PropertyQRectF.cpp" 
    "${CMAKE_CURRENT_SOURCE_DIR}/Core/Core/PropertyEnum.cpp" 
    "${CMAKE_CURRENT_SOURCE_DIR}/Core/Core/PropertyEnumFlags.cpp" 
    "${CMAKE_CURRENT_SOURCE_DIR}/Core/Core/PropertyQSize.cpp" 
    "${CMAKE_CURRENT_SOURCE_DIR}/Core/Core/PropertyQSizeF.cpp" 
    "${CMAKE_CURRENT_SOURCE_DIR}/Core/Core/PropertyQPoint.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Core/Core/PropertyQPointF.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Core/GUI/PropertyQColor.cpp" 
    "${CMAKE_CURRENT_SOURCE_DIR}/Core/GUI/PropertyButton.cpp" 
    "${CMAKE_CURRENT_SOURCE_DIR}/Core/GUI/PropertyQPen.cpp" 
    "${CMAKE_CURRENT_SOURCE_DIR}/Core/GUI/PropertyQBrush.cpp" 
    "${CMAKE_CURRENT_SOURCE_DIR}/Core/GUI/PropertyQFont.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/PropertyWidget/PropertyWidget.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/PropertyWidget/PropertyView.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/PropertyWidget/Utils/InplaceEditing.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/PropertyWidget/Delegates/PropertyDelegateFactory.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/PropertyWidget/Delegates/Core/PropertyDelegateBool.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/PropertyWidget/Delegates/Core/PropertyDelegateInt.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/PropertyWidget/Delegates/Core/PropertyDelegateUInt.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/PropertyWidget/Delegates/Core/PropertyDelegateQString.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/PropertyWidget/Delegates/Core/PropertyDelegateFloat.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/PropertyWidget/Delegates/Core/PropertyDelegateDouble.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/PropertyWidget/Delegates/Core/PropertyDelegateEnum.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/PropertyWidget/Delegates/Core/PropertyDelegateQRect.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/PropertyWidget/Delegates/Utils/PropertyEditorHandler.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/PropertyWidget/Delegates/Core/PropertyDelegateEnumFlags.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/PropertyWidget/Delegates/PropertyDelegate.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/PropertyWidget/Delegates/PropertyDelegateAux.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/PropertyWidget/Delegates/Utils/PropertyDelegatePropertySet.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/PropertyWidget/Delegates/Utils/PropertyDelegateSliderBox.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/PropertyWidget/Delegates/Utils/PropertyDelegateMisc.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/PropertyWidget/Delegates/Utils/PropertyEditorAux.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/PropertyWidget/Delegates/Core/PropertyDelegateQSize.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/PropertyWidget/Delegates/Core/PropertyDelegateQSizeF.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/PropertyWidget/Delegates/Core/PropertyDelegateQPoint.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/PropertyWidget/Delegates/GUI/PropertyDelegateQFont.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/PropertyWidget/Delegates/GUI/PropertyDelegateQColor.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/PropertyWidget/Delegates/GUI/PropertyDelegateButton.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/PropertyWidget/Delegates/GUI/PropertyDelegateQPen.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/PropertyWidget/Delegates/GUI/PropertyDelegateQBrush.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/PropertyWidget/Utils/AccessibilityProxy.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/PropertyWidget/Delegates/Core/PropertyDelegateQPointF.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/PropertyWidget/Delegates/Utils/PropertyDelegateGeoPoint.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/PropertyWidget/Delegates/Utils/PropertyDelegateGeoCoord.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/PropertyWidget/Delegates/Core/PropertyDelegateQRectF.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/PropertyWidget/Delegates/Core/PropertyDelegateQSizeF.cpp"
)

add_library(staticQtnProperty STATIC ${QtnPropertySource}) 

target_include_directories(staticQtnProperty PRIVATE 
	${Qt5Widgets_INCLUDE_DIRS} 
	${Qt5Declarative_INCLUDE_DIRS} 
	${Qt5Script_INCLUDE_DIRS} 
	${CMAKE_CURRENT_SOURCE_DIR} 
	${CMAKE_CURRENT_SOURCE_DIR}/Core
  ${CMAKE_CURRENT_SOURCE_DIR}/PropertyWidget)
	
target_link_libraries(staticQtnProperty PRIVATE ${Qt5Widgets_LIBRARIES} ${Qt5Declarative_LIBRARIES} ${Qt5Script_LIBRARIES})

