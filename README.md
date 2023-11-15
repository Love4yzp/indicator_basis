# Indicator Basis Demo(SqureLine Studio)

This demo is based on Indicator Basis Demo, but made mostly with SqureLine, And set name rules:

screen is the navigator where you focus.
### Home Screen
1. **Labels**: 
   - `homeLblWelcome` - A welcome label on the home screen.
   - `homeLblDate` - A label displaying the current date on the home screen.

2. **Buttons**: 
   - `homeBtnSettings` - A button on the home screen that navigates to the settings screen.
   - `homeBtnInfo` - A button to open the info screen.

3. **Images**:
   - `homeImgLogo` - The application logo displayed on the home screen.
   - `homeImgBanner` - A banner image on the home screen.

### Settings Screen
1. **Labels**:
   - `setLblVolume` - A label indicating the volume control on the settings screen.
   - `setLblBrightness` - A label for brightness adjustment on the settings screen.

2. **Sliders**:
   - `setSliderVolume` - A slider to adjust the volume.
   - `setSliderBrightness` - A slider for brightness control.

3. **Buttons**:
   - `setBtnSave` - A button to save settings.
   - `setBtnCancel` - A cancel button to revert changes.

### Info Screen
1. **Labels**:
   - `infoLblVersion` - A label showing the application version on the info screen.
   - `infoLblCredits` - A label for displaying credits.

2. **Images**:
   - `infoImgDeveloperLogo` - An image of the developer's logo.
   - `infoImgLicense` - An image related to the software license.

3. **Buttons**:
   - `infoBtnContact` - A button to contact support or the development team.
   - `infoBtnClose` - A button to close the info screen.

### Event Handlers
- `onHomeBtnSettingsClick` - Event handler for clicking the settings button on the home screen.
- `onSetSliderBrightnessChange` - Event handler for changes in the brightness slider on the settings screen.
- `onInfoBtnCloseClick` - Event handler for the close button click on the info screen.


### Rules:
(ui_)`TAG_`name(additional info)


标识屏幕：直接就是 Screen 的名字，比如 `ScreenMain`，`ScreenMenu`
标识其它的元素，必须要有某个屏幕的标识：`MainBtnWelcome`
ScreenMain
TimeImgBackground
TimeBtnWifi
TimeLblWifiStatus

Panel: `MainPanel`Pal

TimePnlTimeHour
TimeLblTimeHour
