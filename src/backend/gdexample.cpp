#include "gdexample.h"

#include <godot_cpp/core/class_db.hpp>

using namespace godot;

void AchievementMetadata::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("get_name"), &AchievementMetadata::get_name);
    ClassDB::bind_method(D_METHOD("set_name", "name"), &AchievementMetadata::set_name);
    ClassDB::bind_method(D_METHOD("get_description"), &AchievementMetadata::get_description);
    ClassDB::bind_method(D_METHOD("set_description", "description"), &AchievementMetadata::set_description);

    ADD_PROPERTY(PropertyInfo(Variant::STRING, "name"), "set_name", "get_name");
    ADD_PROPERTY(PropertyInfo(Variant::STRING, "description"), "set_description", "get_description");
}

String AchievementMetadata::get_name() const
{
    return m_name;
}

void AchievementMetadata::set_name(const String& name)
{
    m_name = name;
}

const String& AchievementMetadata::get_description() const
{
    return m_description;
}

void AchievementMetadata::set_description(const String& description)
{
    m_description = description;
}

void GDExample::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("get_achievement_metadata"), &GDExample::get_achievement_metadata);
    ClassDB::bind_method(D_METHOD("set_achievement_metadata", "p_achievement_metadata"), &GDExample::set_achievement_metadata);

    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "achievement_metadata"), "set_achievement_metadata", "get_achievement_metadata");
}

GDExample::GDExample()
//: m_achiMetadata(memnew(AchievementMetadata))
{
    // m_achiMetadata->set_name("TestName");
    // m_achiMetadata->set_description("TestDescription");
}

GDExample::~GDExample() {}

void GDExample::_process(double delta) {}

Ref<AchievementMetadata> GDExample::get_achievement_metadata() const
{
    return m_achiMetadata;
}

void GDExample::set_achievement_metadata(const Ref<AchievementMetadata> achievement_metadata)
{
    m_achiMetadata = achievement_metadata;
}
