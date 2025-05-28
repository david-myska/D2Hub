#include "gdexample.h"
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

void AchievementMetadata::_bind_methods()
{
}

void GDExample::_bind_methods() {
    ClassDB::bind_method(D_METHOD("get_achievement_metadata"), &GDExample::get_achievement_metadata);
    ClassDB::bind_method(D_METHOD("set_achievement_metadata", "p_achievement_metadata"), &GDExample::set_achievement_metadata);

    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "achievement_metadata"), "set_achievement_metadata", "get_achievement_metadata");
}

GDExample::GDExample()
{
    m_achiMetadata.m_name = "TestName";
    m_achiMetadata.m_description = "TestDescription";
}

GDExample::~GDExample() {
}

void GDExample::_process(double delta) {
}

Ref<AchievementMetadata> GDExample::get_achievement_metadata() const {
    return Ref(&m_achiMetadata);
}
void GDExample::set_achievement_metadata(const Ref<AchievementMetadata> achievement_metadata) {
    m_achiMetadata.m_name = achievement_metadata->m_name;
    m_achiMetadata.m_description = achievement_metadata->m_description;
}
