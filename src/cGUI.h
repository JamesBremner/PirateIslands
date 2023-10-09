#include "wex.h"

class cGUI
{
public:
    /** CTOR
     * @param[in] title will appear in application window title
     * @param[in] vlocation set location and size of appplication window
     *
     */
    cGUI(
        const std::string &title,
        const std::vector<int> &vlocation)
        : fm(wex::maker::make())
    {
        fm.move(vlocation);
        fm.text(title);
        constructMenu();

        fm.events().draw(
            [&](PAINTSTRUCT &ps)
            {
                wex::shapes S(ps);
                draw(S);
            });

        fm.show();
        fm.run();
    }
    /** Draw nothing
     *
     * An application should over-ride this method
     * to perform any drawing reuired
     */
    virtual void draw(wex::shapes &S)
    {
        S.text(results,{10,10,400,400});
    }

private:
    wex::gui &fm;
    cInstance thePirateIslands;
    std::string results;

    void constructMenu()
    {
        wex::menubar mbar(fm);
        wex::menu file(fm);
        file.append("calculate",
                    [&](const std::string &title)
                    {
                        wex::filebox fb(fm);
                        thePirateIslands.readfile(fb.open());
                        thePirateIslands.navigate();
                        results = thePirateIslands.textResults();
                        fm.update();
                    });
        mbar.append("File",file);
    }
};