#ifndef RECIPES_RECIPESPLUGIN_H
#define RECIPES_RECIPESPLUGIN_H

#include "../recipespluginglobal.h"

#include <qf/qmlwidgets/framework/plugin.h>

#include <QQmlListProperty>

class QPrinterInfo;

namespace CardReader {
class CardReaderPlugin;
}
namespace Event {
class EventPlugin;
}

namespace Recipes {

class CardChecker;

class RECIPESPLUGIN_DECL_EXPORT RecipesPlugin : public qf::qmlwidgets::framework::Plugin
{
	Q_OBJECT
private:
	typedef qf::qmlwidgets::framework::Plugin Super;
public:
	RecipesPlugin(QObject *parent = nullptr);

	Q_INVOKABLE void previewRecipe(int card_id);
	Q_INVOKABLE bool printRecipe(int card_id, const QPrinterInfo &printer_info);

	Q_INVOKABLE QVariantMap recipeTablesData(int card_id);
private:
	void onInstalled();
	CardReader::CardReaderPlugin* cardReaderPlugin();
	Event::EventPlugin* eventPlugin();

	void previewRecipe_classic(int card_id);
	void printRecipe_classic(int card_id, const QPrinterInfo &printer_info);
};

}

#endif
