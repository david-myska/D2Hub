#ifndef GDEXAMPLE_H
#define GDEXAMPLE_H

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/variant/string.hpp>

namespace godot
{

    class AchievementMetadata : public RefCounted
    {
        GDCLASS(AchievementMetadata, RefCounted)

        String m_name;
        String m_description;

    protected:
        static void _bind_methods();

    public:
        String get_name() const;
        void set_name(const String& name);

        const String& get_description() const;
        void set_description(const String& name);
    };

    class GDExample : public Node
    {
        GDCLASS(GDExample, Node)

        Ref<AchievementMetadata> m_achiMetadata;

    protected:
        static void _bind_methods();

    public:
        GDExample();
        ~GDExample();

        void _process(double delta) override;

        Ref<AchievementMetadata> get_achievement_metadata() const;
        void set_achievement_metadata(const Ref<AchievementMetadata> achievement_metadata);
    };

}

#endif