#include <Geode/Geode.hpp>
#include <Geode/modify/EditorUI.hpp>

#include <algorithm>
#include <cctype>
#include <string>
#include <vector>

using namespace geode::prelude;

namespace {
    enum class DecorationStyle {
        Auto,
        Castle,
        Forest,
        City,
        Circuit,
        Space,
        Arch
    };

    struct Offset {
        float x;
        float y;
    };

    std::string lowerCopy(std::string value) {
        std::transform(value.begin(), value.end(), value.begin(), [](unsigned char c) {
            return static_cast<char>(std::tolower(c));
        });
        return value;
    }

    const char* styleName(DecorationStyle style) {
        switch (style) {
            case DecorationStyle::Auto: return "Auto";
            case DecorationStyle::Castle: return "Castle";
            case DecorationStyle::Forest: return "Forest";
            case DecorationStyle::City: return "City";
            case DecorationStyle::Circuit: return "Circuit";
            case DecorationStyle::Space: return "Space";
            default: return "Arch";
        }
    }

    DecorationStyle detectStyle(const std::string& prompt) {
        const auto text = lowerCopy(prompt);
        if (text.find("castle") != std::string::npos || text.find("ruin") != std::string::npos ||
            text.find("tower") != std::string::npos || text.find("fort") != std::string::npos) {
            return DecorationStyle::Castle;
        }
        if (text.find("forest") != std::string::npos || text.find("tree") != std::string::npos ||
            text.find("nature") != std::string::npos || text.find("wood") != std::string::npos) {
            return DecorationStyle::Forest;
        }
        if (text.find("city") != std::string::npos || text.find("building") != std::string::npos ||
            text.find("skyline") != std::string::npos || text.find("urban") != std::string::npos) {
            return DecorationStyle::City;
        }
        if (text.find("circuit") != std::string::npos || text.find("cyber") != std::string::npos ||
            text.find("tech") != std::string::npos || text.find("neon") != std::string::npos) {
            return DecorationStyle::Circuit;
        }
        if (text.find("space") != std::string::npos || text.find("star") != std::string::npos ||
            text.find("galaxy") != std::string::npos || text.find("planet") != std::string::npos) {
            return DecorationStyle::Space;
        }
        return DecorationStyle::Arch;
    }

    void addOffset(std::vector<Offset>& offsets, float x, float y) {
        offsets.push_back({x * 30.f, y * 30.f});
    }

    std::vector<Offset> createPattern(DecorationStyle requestedStyle, const std::string& prompt, int limit) {
        const auto style = requestedStyle == DecorationStyle::Auto ? detectStyle(prompt) : requestedStyle;
        std::vector<Offset> offsets;
        offsets.reserve(static_cast<size_t>(limit));

        switch (style) {
            case DecorationStyle::Castle: {
                // A wall with battlements and two towers.
                for (int x = -4; x <= 4; ++x) {
                    addOffset(offsets, static_cast<float>(x), -1.f);
                    addOffset(offsets, static_cast<float>(x), 0.f);
                }
                for (int y = 1; y <= 3; ++y) {
                    addOffset(offsets, -4.f, static_cast<float>(y));
                    addOffset(offsets, 4.f, static_cast<float>(y));
                }
                for (int x = -5; x <= -3; x += 2) addOffset(offsets, static_cast<float>(x), 4.f);
                for (int x = 3; x <= 5; x += 2) addOffset(offsets, static_cast<float>(x), 4.f);
                for (int x = -3; x <= 3; x += 2) addOffset(offsets, static_cast<float>(x), 2.f);
                break;
            }
            case DecorationStyle::Forest: {
                // Three blocky treetops and trunks, arranged as a little forest.
                for (int tree = 0; tree < 3; ++tree) {
                    const float center = static_cast<float>((tree - 1) * 4);
                    for (int row = 0; row < 4; ++row) {
                        for (int col = -row; col <= row; ++col) {
                            if ((row + col) % 2 == 0) addOffset(offsets, center + col, 2.f + row);
                        }
                    }
                    addOffset(offsets, center, 1.f);
                    addOffset(offsets, center, 0.f);
                }
                break;
            }
            case DecorationStyle::City: {
                // Five skyline towers, with repeated floors and stepped roofs.
                const int heights[] = {3, 5, 2, 4, 3};
                for (int building = 0; building < 5; ++building) {
                    const float left = static_cast<float>(building * 3 - 6);
                    const int height = heights[building];
                    for (int y = 0; y < height; ++y) {
                        addOffset(offsets, left, static_cast<float>(y));
                        addOffset(offsets, left + 1.f, static_cast<float>(y));
                    }
                    addOffset(offsets, left, static_cast<float>(height));
                    addOffset(offsets, left + 1.f, static_cast<float>(height));
                }
                break;
            }
            case DecorationStyle::Circuit: {
                // A circuit-board motif: rails, vertical traces, and terminal blocks.
                for (int x = -5; x <= 5; ++x) {
                    addOffset(offsets, static_cast<float>(x), -1.f);
                    addOffset(offsets, static_cast<float>(x), 3.f);
                }
                for (int x : {-4, -1, 2, 5}) {
                    for (int y = 0; y <= 2; ++y) addOffset(offsets, static_cast<float>(x), static_cast<float>(y));
                }
                break;
            }
            case DecorationStyle::Space: {
                // Nested square orbit / star-frame motif.
                for (int x = -4; x <= 4; ++x) {
                    addOffset(offsets, static_cast<float>(x), -3.f);
                    addOffset(offsets, static_cast<float>(x), 3.f);
                }
                for (int y = -2; y <= 2; ++y) {
                    addOffset(offsets, -4.f, static_cast<float>(y));
                    addOffset(offsets, 4.f, static_cast<float>(y));
                }
                for (int x = -2; x <= 2; ++x) {
                    addOffset(offsets, static_cast<float>(x), -1.f);
                    addOffset(offsets, static_cast<float>(x), 1.f);
                }
                addOffset(offsets, 0.f, 0.f);
                addOffset(offsets, -6.f, 0.f);
                addOffset(offsets, 6.f, 0.f);
                break;
            }
            default: {
                // A compact symmetric arch with a floating top accent.
                for (int x = -4; x <= 4; ++x) addOffset(offsets, static_cast<float>(x), -2.f);
                for (int y = -1; y <= 2; ++y) {
                    addOffset(offsets, -4.f, static_cast<float>(y));
                    addOffset(offsets, 4.f, static_cast<float>(y));
                }
                for (int x = -3; x <= 3; ++x) {
                    if (x % 2 == 0) addOffset(offsets, static_cast<float>(x), 3.f);
                }
                addOffset(offsets, 0.f, 4.f);
                addOffset(offsets, -1.f, 3.f);
                addOffset(offsets, 1.f, 3.f);
                break;
            }
        }

        // Ensure a predictable object count without allowing oversized generations.
        if (offsets.size() > static_cast<size_t>(limit)) offsets.resize(static_cast<size_t>(limit));
        return offsets;
    }

    CCLabelBMFont* makeLabel(const char* text, float scale = 0.42f) {
        auto label = CCLabelBMFont::create(text, "bigFont.fnt");
        if (label) label->setScale(scale);
        return label;
    }

    CCMenuItemSpriteExtra* makeButton(const char* text, CCObject* target, SEL_MenuHandler callback, float scale = 0.42f) {
        auto label = makeLabel(text, scale);
        if (!label) return nullptr;
        return CCMenuItemSpriteExtra::create(label, target, callback);
    }
}

class R387DecoratorPopup : public geode::Popup {
protected:
    LevelEditorLayer* m_level = nullptr;
    CCTextInputNode* m_promptInput = nullptr;
    CCLabelBMFont* m_styleLabel = nullptr;
    CCLabelBMFont* m_countLabel = nullptr;
    CCLabelBMFont* m_statusLabel = nullptr;
    DecorationStyle m_style = DecorationStyle::Auto;
    int m_limit = 24;
    bool m_previewReady = false;
    std::string m_previewPrompt;
    DecorationStyle m_previewStyle = DecorationStyle::Auto;
    int m_previewLimit = 24;
    std::vector<GameObject*> m_lastBatch;

    bool init(LevelEditorLayer* level) {
        if (!Popup::init(380.f, 286.f)) return false;
        m_level = level;
        this->setTitle("R387 AI Decorator");

        auto description = makeLabel("Describe a theme; preview it, then generate solid blocks.", 0.36f);
        if (description) {
            description->setPosition({190.f, 224.f});
            m_mainLayer->addChild(description);
        }

        m_promptInput = CCTextInputNode::create(310.f, 30.f, "e.g. ancient castle with towers", "bigFont.fnt");
        if (m_promptInput) {
            m_promptInput->setPosition({190.f, 184.f});
            m_mainLayer->addChild(m_promptInput);
        }

        m_styleLabel = makeLabel("Style: Auto (detect from prompt)", 0.4f);
        if (m_styleLabel) {
            m_styleLabel->setPosition({190.f, 148.f});
            m_mainLayer->addChild(m_styleLabel);
        }
        m_countLabel = makeLabel("Object limit: 24", 0.4f);
        if (m_countLabel) {
            m_countLabel->setPosition({190.f, 126.f});
            m_mainLayer->addChild(m_countLabel);
        }

        auto menu = CCMenu::create();
        if (!menu) return false;
        menu->setPosition({0.f, 0.f});

        auto styleButton = makeButton("Change style", this, menu_selector(R387DecoratorPopup::onStyle), 0.38f);
        if (styleButton) {
            styleButton->setPosition({100.f, 92.f});
            menu->addChild(styleButton);
        }
        auto countButton = makeButton("Change limit", this, menu_selector(R387DecoratorPopup::onLimit), 0.38f);
        if (countButton) {
            countButton->setPosition({280.f, 92.f});
            menu->addChild(countButton);
        }
        auto previewButton = makeButton("Preview", this, menu_selector(R387DecoratorPopup::onPreview), 0.45f);
        if (previewButton) {
            previewButton->setPosition({105.f, 52.f});
            menu->addChild(previewButton);
        }
        auto generateButton = makeButton("Generate", this, menu_selector(R387DecoratorPopup::onGenerate), 0.45f);
        if (generateButton) {
            generateButton->setPosition({270.f, 52.f});
            menu->addChild(generateButton);
        }
        auto undoButton = makeButton("Undo last batch", this, menu_selector(R387DecoratorPopup::onUndo), 0.35f);
        if (undoButton) {
            undoButton->setPosition({190.f, 28.f});
            menu->addChild(undoButton);
        }
        m_mainLayer->addChild(menu);

        m_statusLabel = makeLabel("Offline generator | blocks are solid", 0.34f);
        if (m_statusLabel) {
            m_statusLabel->setPosition({190.f, 8.f});
            m_mainLayer->addChild(m_statusLabel);
        }
        this->refreshLabels();
        return true;
    }

    void refreshLabels() {
        if (m_styleLabel) {
            std::string text = "Style: ";
            text += styleName(m_style);
            if (m_style == DecorationStyle::Auto) text += " (detect from prompt)";
            m_styleLabel->setString(text.c_str());
        }
        if (m_countLabel) {
            std::string text = "Object limit: " + std::to_string(m_limit);
            m_countLabel->setString(text.c_str());
        }
    }

    std::string promptText() const {
        if (!m_promptInput) return {};
        const char* raw = m_promptInput->getString();
        return raw ? std::string(raw) : std::string();
    }

public:
    static R387DecoratorPopup* create(LevelEditorLayer* level) {
        auto ret = new R387DecoratorPopup();
        if (ret && ret->init(level)) {
            ret->autorelease();
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }

    void onStyle(CCObject*) {
        switch (m_style) {
            case DecorationStyle::Auto: m_style = DecorationStyle::Castle; break;
            case DecorationStyle::Castle: m_style = DecorationStyle::Forest; break;
            case DecorationStyle::Forest: m_style = DecorationStyle::City; break;
            case DecorationStyle::City: m_style = DecorationStyle::Circuit; break;
            case DecorationStyle::Circuit: m_style = DecorationStyle::Space; break;
            default: m_style = DecorationStyle::Auto; break;
        }
        m_previewReady = false;
        this->refreshLabels();
        if (m_statusLabel) m_statusLabel->setString("Style changed. Preview again before generating.");
    }

    void onLimit(CCObject*) {
        if (m_limit == 12) m_limit = 24;
        else if (m_limit == 24) m_limit = 36;
        else if (m_limit == 36) m_limit = 48;
        else m_limit = 12;
        m_previewReady = false;
        this->refreshLabels();
        if (m_statusLabel) m_statusLabel->setString("Limit changed. Preview again before generating.");
    }

    void onPreview(CCObject*) {
        const auto prompt = promptText();
        if (prompt.empty()) {
            if (m_statusLabel) m_statusLabel->setString("Type a theme first, then preview it.");
            return;
        }
        const auto chosen = m_style == DecorationStyle::Auto ? detectStyle(prompt) : m_style;
        const auto plan = createPattern(m_style, prompt, m_limit);
        m_previewReady = !plan.empty();
        m_previewPrompt = prompt;
        m_previewStyle = m_style;
        m_previewLimit = m_limit;
        std::string text = "Preview: ";
        text += styleName(chosen);
        text += " | ";
        text += std::to_string(plan.size());
        text += " solid blocks. Click Generate to place them.";
        if (m_statusLabel) m_statusLabel->setString(text.c_str());
    }

    void onGenerate(CCObject*) {
        if (!m_level || !m_promptInput) {
            if (m_statusLabel) m_statusLabel->setString("Editor is unavailable. Close this and reopen it in the editor.");
            return;
        }
        const auto prompt = promptText();
        if (!m_previewReady || prompt != m_previewPrompt || m_style != m_previewStyle || m_limit != m_previewLimit) {
            m_previewReady = false;
            if (m_statusLabel) m_statusLabel->setString("Preview first, and preview again after changing the prompt or settings.");
            return;
        }

        const auto plan = createPattern(m_style, prompt, m_limit);
        if (plan.empty()) {
            if (m_statusLabel) m_statusLabel->setString("This prompt did not produce a pattern. Try another theme.");
            return;
        }

        CCPoint anchor = {300.f, 150.f};
        if (m_level->m_editorUI && m_level->m_editorUI->m_selectedObject) {
            const auto selectedPosition = m_level->m_editorUI->m_selectedObject->getPosition();
            anchor = {selectedPosition.x + 180.f, selectedPosition.y + 30.f};
        }

        m_lastBatch.clear();
        int added = 0;
        for (const auto& offset : plan) {
            auto object = GameObject::createWithKey(1);
            if (!object) continue;
            object->setPosition({anchor.x + offset.x, anchor.y + offset.y});
            m_level->addObject(object);
            m_level->updateObjectSection(object);
            m_lastBatch.push_back(object);
            ++added;
        }

        m_previewReady = false;
        std::string status = "Placed ";
        status += std::to_string(added);
        status += " blocks. They are solid gameplay objects.";
        if (m_statusLabel) m_statusLabel->setString(status.c_str());
        log::info("R387 AI Decorator placed {} block objects from prompt: {}", added, prompt);
    }

    void onUndo(CCObject*) {
        if (!m_level || m_lastBatch.empty()) {
            if (m_statusLabel) m_statusLabel->setString("No generated batch to remove.");
            return;
        }

        int removed = 0;
        auto allObjects = m_level->getAllObjects();
        for (auto object : m_lastBatch) {
            if (!object || !allObjects) continue;
            bool isStillInLevel = false;
            for (unsigned int i = 0; i < allObjects->count(); ++i) {
                if (allObjects->objectAtIndex(i) == object) {
                    isStillInLevel = true;
                    break;
                }
            }
            if (isStillInLevel) {
                m_level->removeObject(object, true);
                ++removed;
            }
        }
        m_lastBatch.clear();
        if (m_statusLabel) {
            std::string status = "Removed ";
            status += std::to_string(removed);
            status += " objects from the last batch.";
            m_statusLabel->setString(status.c_str());
        }
    }
};

class $modify(R387EditorUI, EditorUI) {
    bool init(LevelEditorLayer* editorLayer) {
        if (!EditorUI::init(editorLayer)) return false;

        auto sprite = CCLabelBMFont::create("R387 AI", "bigFont.fnt");
        if (!sprite) return true;
        sprite->setScale(0.55f);

        auto button = CCMenuItemSpriteExtra::create(
            sprite,
            this,
            menu_selector(R387EditorUI::onR387Decorator)
        );
        if (!button) return true;
        button->setID("r387-ai-decorator-button");

        auto menu = CCMenu::create();
        if (!menu) return true;
        menu->addChild(button);
        menu->setID("r387-ai-decorator-menu");
        menu->setPosition({30.f, 120.f});
        this->addChild(menu, 1000);
        return true;
    }

    void onR387Decorator(CCObject*) {
        auto popup = R387DecoratorPopup::create(m_editorLayer);
        if (popup) popup->show();
        else FLAlertLayer::create("R387 AI Decorator", "Couldn't open the decorator panel.", "OK")->show();
    }
};
