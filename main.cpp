\
    #include <Geode/Geode.hpp>
    #include <Geode/modify/EditorUI.hpp>
    #include <Geode/ui/Popup.hpp>
    #include <Geode/utils/web.hpp>
    #include <algorithm>
    #include <string>
    #include <vector>

    using namespace geode::prelude;

    namespace r387 {
        // The generation model is intentionally separate from editor placement.
        // This local planner is a safe prototype, not an LLM: it creates a deterministic
        // decoration recipe. An AI backend can later return the same recipe format.
        struct DecorationSpec {
            std::string prompt;
            int maxObjects = 750;
            int detailPercent = 75;
            std::string style = "Realistic";
        };

        struct PlannedObject {
            int objectID;
            float x;
            float y;
            float scale;
            float rotation;
        };

        static std::vector<PlannedObject> makeLocalPlan(
            DecorationSpec const& spec, CCPoint origin, CCSize area
        ) {
            std::vector<PlannedObject> result;
            int limit = std::clamp(spec.maxObjects, 1, 750);

            // Prototype palette: native GD block/deco object IDs. Exact IDs can vary
            // in usefulness by game version; this recipe is a starting point and needs
            // in-game visual validation before release.
            std::vector<int> palette = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
            unsigned int seed = 387u;
            for (unsigned char c : spec.prompt) seed = seed * 33u + c;

            auto next = [&seed]() -> unsigned int {
                seed = seed * 1664525u + 1013904223u;
                return seed;
            };

            int count = std::min(limit, std::max(24, spec.detailPercent * 7));
            result.reserve(static_cast<size_t>(count));
            for (int i = 0; i < count; ++i) {
                auto id = palette[next() % palette.size()];
                float px = origin.x + static_cast<float>(next() % 10000) / 10000.f * area.width;
                float py = origin.y + static_cast<float>(next() % 10000) / 10000.f * area.height;
                float scale = 0.65f + static_cast<float>(next() % 90) / 100.f;
                float rot = static_cast<float>(next() % 360);
                result.push_back({id, px, py, scale, rot});
            }
            return result;
        }

        // This is the single seam where the editor-specific object insertion adapter
        // belongs. Do not pretend generation succeeded until objects are actually
        // inserted into the active EditorLayer and can be undone through GD's editor.
        static void showPrototypeNotice() {
            FLAlertLayer::create(
                "R387 AI Decorator — prototype",
                "The decorator UI and local planning scaffold are present. "
                "Native editor-object insertion and selection bounds still need to be "
                "connected and validated against your installed Geode SDK / GD version. "
                "This build does not yet place objects into a level.",
                "OK"
            )->show();
        }
    }

    class R387DecoratorPopup : public geode::Popup<> {
    protected:
        CCLabelBMFont* m_promptLabel = nullptr;
        CCLabelBMFont* m_limitLabel = nullptr;
        CCLabelBMFont* m_detailLabel = nullptr;
        int m_limit = 750;
        int m_detail = 75;
        std::string m_prompt = "Ancient stone wall covered in moss, with a shattered window";

        bool setup() override {
            this->setTitle("R387 AI Decorator");

            auto winSize = CCDirector::sharedDirector()->getWinSize();
            auto title = CCLabelBMFont::create(
                "Prompt-driven level decoration", "bigFont.fnt"
            );
            title->setScale(0.42f);
            title->setPosition({m_size.width / 2, m_size.height - 48});
            m_mainLayer->addChild(title);

            auto prompt = CCLabelBMFont::create(
                "Current prompt: mossy stone wall + shattered window", "chatFont.fnt"
            );
            prompt->setScale(0.42f);
            prompt->setPosition({m_size.width / 2, m_size.height - 88});
            m_mainLayer->addChild(prompt);

            m_limitLabel = CCLabelBMFont::create("Object limit: 750", "bigFont.fnt");
            m_limitLabel->setScale(0.45f);
            m_limitLabel->setPosition({m_size.width / 2, m_size.height - 122});
            m_mainLayer->addChild(m_limitLabel);

            m_detailLabel = CCLabelBMFont::create("Detail: 75%  |  Style: Realistic", "bigFont.fnt");
            m_detailLabel->setScale(0.45f);
            m_detailLabel->setPosition({m_size.width / 2, m_size.height - 150});
            m_mainLayer->addChild(m_detailLabel);

            auto explain = CCLabelBMFont::create(
                "Prototype only: placement adapter is not connected yet.", "chatFont.fnt"
            );
            explain->setScale(0.42f);
            explain->setPosition({m_size.width / 2, m_size.height - 190});
            m_mainLayer->addChild(explain);

            auto closeBtn = CCMenuItemSpriteExtra::create(
                CCSprite::createWithSpriteFrameName("GJ_closeBtn_001.png"),
                this,
                menu_selector(R387DecoratorPopup::onClose)
            );
            auto menu = CCMenu::create();
            menu->addChild(closeBtn);
            menu->setPosition({m_size.width / 2, 22});
            m_mainLayer->addChild(menu);

            return true;
        }

        void onClose(CCObject*) {
            this->onClose(nullptr);
        }

    public:
        static R387DecoratorPopup* create() {
            auto ret = new R387DecoratorPopup();
            if (ret && ret->initAnchored(360.f, 235.f)) {
                ret->autorelease();
                return ret;
            }
            CC_SAFE_DELETE(ret);
            return nullptr;
        }
    };

    class $modify(R387EditorUI, EditorUI) {
        bool init(LevelEditorLayer* editorLayer) {
            if (!EditorUI::init(editorLayer)) return false;

            // The button is a prototype entry point. Current stable APIs differ across
            // Geode/GD versions; if this hook fails to compile, regenerate the project
            // against the installed SDK and adjust the EditorUI signature.
            auto sprite = CCLabelBMFont::create("AI", "bigFont.fnt");
            sprite->setScale(0.55f);
            auto button = CCMenuItemSpriteExtra::create(
                sprite,
                this,
                menu_selector(R387EditorUI::onR387Decorator)
            );
            button->setID("r387-ai-decorator-button");

            auto menu = CCMenu::create();
            menu->addChild(button);
            menu->setID("r387-ai-decorator-menu");
            menu->setPosition({30.f, 120.f});
            this->addChild(menu, 1000);

            return true;
        }

        void onR387Decorator(CCObject*) {
            auto popup = R387DecoratorPopup::create();
            if (popup) popup->show();
        }
    };
