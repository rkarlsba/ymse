#pragma once

static const char* szVEROROUTE_VERSION =

//"0.91";	// Fixed bad write to pdf. Allow backspace to work as delete.
//"0.92";	// Add Ctrl shortcuts for File menu options. Changed tutorials.  Fixed origId bug.
//"0.93";	// Try auto-generate unique Name when adding new part.
//"0.94";	// Added B.O.M.
//"0.95";	// Added save to PNG.
//"0.96";	// Show File menu-shortcuts.  Fixed tutorial description of polarised component Export.
//"0.97";	// Added new part types.  Improved TO-92 outline.  Auto-centre when writing to PDF.
//"0.98";	// Added Undo/Redo.
//"0.981";	// Bug fix: Undo/Redo.
//"0.99";	// Added new part types including multi-pole switches.
//"0.991";	// Bug fix: Redraw when toggling track styles.
//"0.992";	// Improve routing.
//"0.993";	// Routing improvement.
//"0.994";	// Bug fix: Erasing under floating pins while auto-routing.
//"0.995";	// Check if circuit has been saved before launching Tutorials. Don't auto-raise Info window on Undo/Redo.
//"0.996";	// Bug fix: Deleting grouped components could cause a crash later. Allow DIP gap resizing with "E","R" keys.
//"0.997";	// Added part type combo.  Added width buttons for DIP gap resizing instead of "E","R" keys.
//"0.998";	// Bug fix: After "Paste" auto-routed wire, color should stay on board when wire moved.
//"0.999";	// Bug fix: Fix possible crash if adding new wire while auto-routing is enabled.
//"1.00";	// Disable component text buttons when appropriate.  Tidy code.  Drop Beta.
//"1.01";	// Added scroll bars. Place new components in top-left of visible view and float them if needed.
//"1.10";	// Added import of netlist from TinyCAD schematic. Hide old Import/Export.
//"1.11";	// Fixed crash on importing unknown package. Increased maximum number of pins per component to over 200.
//"1.12";	// Fixed TinyCAD package names for NP capacitors. Make netlist import allow spaces in Name and Value fields.
//"1.13";	// Bug fixes: PDF write was broken since V1.10.  "Add" menu item was enabled when view was mirrored.
//"1.14";	// Added Help menu item to check if a new version is available.
//"1.15";	// Added more Edit menu items.  Check for Tutorials and History folders at start-up.
//"1.16";	// Fixed mouse-wheel behaviour on windows.  Added View menu option to show IC pins as numbers.
//"1.17";	// Added ground-fill.
//"1.18";	// Bug fix: Info dialog edits weren't treated as changes so "Save" was broken, but "Save As" was OK.
//"1.19";	// Add separate "Paste" and "Paste+Tidy" buttons for auto-routing.  Updated tutorials.
//"1.20";	// Added Grid checkbox.  PDF shows bounding box and can show grid.
//			// Increased max pins per component to 255.
//			// When writing to file, append ". vrt", ".pdf", ".png" to filename as needed.
//"1.21";	// Improved rendering speed by avoiding pointless repaints.
//			// Control dialog rearranged so a checkbox controls ground-fill.
//			// Draw foil capacitors with thinner shape to better match the footprint.
//			// Draw floating component text in red.
//			// Made export to PDF centre on grid bounds instead of circuit bounds.
//			// Write status bar messages.
//"1.22";	// Added command line options to specify vrt file and path to VeroRoute home directory.
//			// Support drag and drop of vrt files from file explorer.
//			// Allow multiple VeroRoute instances to run at the same time.
//			// Improved error reporting during netlist import.
//			// Added color saturation slider.
//			// Changed rendering behaviour when clicking on an item in the Broken list.
//			// Added gEDA symbol libraries.
//"1.23";	// Fixed memory leak on shutdown.
//			// Updated Tutorial 19 with info on creating a registry key for MS Windows.
//			// Added more JFETs to the symbol libraries.
//"1.24";	// Added IC Pin Labels dialog.
//"1.25";	// Usability tweaks:
//			// Slow down the grid auto-pan/resize when moving components with the mouse.
//			// Selecting a NodeID in the "Broken" list should also highlight it in the "Floating" list if it's there.
//			// Added a Spin FV-1 IC and serial EEPROM to the gEDA library.
//"1.26";	// Bug fix: Holding down "P" and SPACE at the same time would allow mouse move to modify component pins.
//			// Usability tweak: Holding down SPACE now allows un-painting the board under a placed component pin,
//			// or painting the board under the pin to match the existing NodeId on the pin.
//"1.27";	// Bug fix: Deleting a component was not immediately updating the Bad Nodes lists.
//			// Bug fix: V1.26 allowed components to be dragged while user was painting pins.
//"1.28";	// Improvements:
//			// Added "Toggle Grid" and "Toggle Mirror" as View menu items.
//			// List part types in B.O.M.
//			// Allow pin labels for TO packages as well as DIPs, SIPs.
//			// New feature: Added "Parts/Templates Dialog" so user can build a parts library
//			// for ICs and TO packages (avoids defining pin labels with each new circuit).
//			// Double-clicking on any listed part adds it to the circuit.
//"1.29";	// Added Crystal component type and gEDA library symbol.
//"1.30";	// Improvement: Duplicating components copies their Value too if it's different to the name.
//			// Added more component types. (Relays, Switches, Trimpots).
//			// Changed default lengths of resistors and diodes to 400 mil.
//			// Updated gEDA/TinyCAD import code for RESISTOR/DIODE/CAP_CERAMIC/CAP_FILM
//			// so that appending an optional integer number to the footprint name
//			// sets the length in 100s of mil. (e.g. RESISTOR3 = 300mil)
//			// Updated gEDA library with resistors and caps of different lengths.
//			// Added ability to select components/wipe tracks within area (hold down "R" to draw areas).
//"1.31";	// Improvement: User-defined areas select tracks as well as components.
//"1.32";	// New feature: "File->Open (merge into current)".
//"1.33";	// New feature: Allow horizontal veroboard strips.
//			// Tweaked color assignment algorithm.
//"1.34";	// Bug fix: Merge was not clearing existing user-defined areas.
//			// New feature: Added margin control for auto-crop.
//			// New feature: While the user draws a rectangle, show its size in the status bar.
//"1.35";	// New feature: Replace "Mirror" with "Flip-H" and "Flip-V" options.
//			// New feature: Add->Text menu item for putting labels on the grid.
//"1.36";	// Bug fix: Buttons in Text Label Editor weren't always reflecting text style.
//			// Bug fix: Prevent user from selecting text boxes in "Hide Text" mode.
//			// New feature: Added painting by "flood-fill".
//"1.37";	// Bug fix: Trying to undo "Delete text" could cause a crash.
//			// New feature: Text color can now be chosen.
//			// New feature: Hitting "V" key will copy+paste a selected text box.
//"1.38";	// Added more component types: TO-39 package, pin strips, terminal blocks, inductor, wide film cap, fuse holder, relays.
//"1.39";	// Moved some controls from the control dialog to the toolbar.  Reduced heights of control dialog and templates dialog.
//"1.40";	// Minor improvements: Automatically delete textboxes with no text.
//			// Show board size in mm in the window title bar.  Show drawn rectangle size in mm in the status bar.
//			// Color toolbar icons to differentiate track styles / diagonal modes.
//"1.41";	// Bug fix: Menu and toolbars were being blanked while drawing rectangles with the "R" key.
//			// Bug fix: The status of the diagonals mode was not always restored when toggling between vero and non-vero styles.
//			// Improvement: Prevent copying of textboxes with no text.
//			// Improvement: Show text box outline in dark grey instead of black, so it can be seen in ground-fill mode.
//			// Improvement: When zooming in/out, try to keep the view showing the same centre point.
//			// Improvement: Draw pin labels in black instead of grey so they are easier to read.
//"1.42";	// Improvement: Added Rendering dialog controls for component text size.
//			// Bug fix: Correct footprints for GTR-2 relays.
//			// Improved outline sizes for relays and Bourns trimpots.
//"1.43";	// Bug fix: Stop potential crash when defining areas for part/track selection.
			// Improvement: Add toolbar button to select part/tracks by area.
//"1.44";	// Bug fix: Fixed bad TRCD Relay.
//			// New feature: Added component editor.
//"1.45";	// Bug fix: Prevent mouse from dragging shapes off screen in the component editor.
//			// Bug fix: Fix shape selection in the component editor.
//"1.46";	// Bug fix: Fixed error in loaded old VRTs with strip connectors.
//"1.47";	// Bug fix: Version 1.46 would produce duplicate shapes when loading parts from VRT.
//			// Bug fix: Changing DIP width wasn't changing the component outline.
//			// New feature: Component editor can define "holes" (i.e. non-paintable grid points).
//"1.48";	// Bug fix: Possible crash during auto-routing while moving wires.
//			// Improvement: Speed up algorithm for routing/connectivity checking.
//"1.49";	// Restrict labels to 2 orientations. (Left to right, or bottom to top).
//			// Improvement: Add rendering options to show target board area.
//"1.50";	// Bug fix: Clicking on a floating track pattern should not place floating components.
//			// Moved the Pad/Track/Hole/Gap controls to the Rendering Options dialog.  Updated the tutorials.
//"1.51";	// Bug fix: Floating track pattern could not be placed over unpainted wires on the board.
//			// Improvement: Made buttons to move component labels use a smaller step size.
//"1.52";	// Bug fix: Missing Templates folder would disable Undo/Redo.
//			// Bug fix: Changing component type using Control Dialog (e.g. from Diode to LED) not working properly.
//			// History and Templates folders now auto-created in OS/application specific locations.
//"1.53";	// Improvement: Added Undo/Redo buttons to toolbar.
//			// Improvement: Allow Undo/Redo in Component Editor mode.
//"1.54";	// Added Zoom buttons to toolbar.
//			// Changed executable name, folder names, and file names to lowercase.
//			// Made program search for "tutorials" folder in standard locations.
//"1.55";	// Improvement: List recent VRT files in File menu.
//"1.56";	// Tweaked dialog layouts to better handle 11 point fonts.
//"1.57";	// Improvement: Faster routing algorithm.
//"1.58";	// Improvement: Faster routing and connectivity checking.
//"1.59";	// Improvement: Added Key/Mouse Actions dialog under Help menu.
//"1.60";	// Bug fix: Since V1.58, connected tracks were not always rendered properly.
//			// Improvement: Added option to disable "Fast" routing (to try reduce Bad Nodes).
//"1.61";	// Improvement: Auto-router respects manually painted track between two pins.
//			// Allow manual over-painting of auto-routed track, and show
//			// all auto-routed tracks with hatched lines (including the current selection).
//			// Split "Paste" and "Tidy" buttons, and only allow "Tidy" on completed circuits.
//"1.62";	// Bug fix: Don't allow parts/tracks selected by area to be manipulated if a control slider is hiding some of the selection.
//			// Bug fix: Don't show selected area if both control sliders are moved to Off.
//			// New feature: Allow wires to cross, and allow up to two wires to be stacked next to each other (but no hole sharing yet).
//"1.63";	// Bug fix: 1.62 could crash or corrupt the layout if stacking wires under an IC.
//"1.64";	// Bug fix: Fixed algorithm for stacking wires.
//			// New feature: Allow 2 wires to share a hole.
//			// Added "Wire Options" dialog to set allowed wire operations.
//"1.65";	// Bug fix: Wire hole-sharing under an IC only worked with wire-crossing enabled.
//			// Improvement: Render wires as rounded rectangles so hole-sharing looks clearer.
//"1.66";	// New feature: Allow shapes to be filled with colors.
//"1.67";	// Bug fix: Rotating a track pattern did not correctly handle competing diagonals.
//			// Bug fix: Couldn't use keys to move/rotate a track pattern unless components were also selected.
//"1.68";	// Bug fix: Component Editor not drawing footprint area with dashed line.
//"1.69";	// New feature: Allow netlist import in OrCAD2 format (so KiCad can be used as schematic editor).
//"1.70";	// Bug fix: Handle selection of rotated shapes in component editor.
			// Bug fix: Make netlist import handle connectors (strip/blocks) with different numbers of pins.
//"1.71";	// Bug fix: Component editor not checking connectors (strip/blocks) for reserved import strings.
//"1.72";	// Bug fix: Flood fill not preserving status of grid under wires.
//"1.73";	// New feature: Make pin label alignment (Left,Centre,Right) editable in pin labels dialog.
//"1.74";	// Better treatment of part labels under part rotation.  Offset labels for Pads/Connectors by default.
//"1.75";	// Partial bug fix: Rendering components to PDF would always show them filled (even if "Fill" slider was off).
			// Fixed to show only outline if "Fill" slider is off.
			// New feature: Added info to rendering dialog showing the minimum guaranteed track separation
			// based on the current (pad, track, gap, diagonal mode, ground fill) settings.
			// New feature: Allow Gerber export when in Mono mode (Beta).
//"1.76";	// Improvement: In Mono mode, all text boxes are now automatically mirrored for toner-transfer.
			// Improvement: In Mono mode, all component outlines and component text are rendered to match Gerber output.
//"1.77";	// Bug fix: Text rendering to PDF was broken by changes for PCB text.
			// Improvement: Text size setting in Renderer Dialog affects PCB text size also.
			// Improvement: Added more PCB parameters (for Gerber) to Rendering Dialog.
			// Moved all Gerber related viewing from Mono mode to new PCB mode on the Track slider.
//"1.78";	// Bug fix: Unpainted wire ends were being drawn as painted.
			// Improvement: Added default label offsets to small Electrolytic Caps so they don't lie over pin holes.
			// Stop offsetting pad labels by default (it was too confusing manipulating them like that).
//"1.79";	// New feature: File menu allows Gerber export as either 1-layer or 2-layer.
			// Removed NPTH/PTH selection since it is implied by the export type (1-layer/2-layer).
			// Changed hatched lines in Mono mode from yellow to red.
			// Hide vias in PCB mode (as they are ignored at the moment).
			// Help->Support now opens forum in browser.
			// Added "Clear recent files list" to File menu.
//"1.80";	// Added second board layer.
			// Made the Control and Component Editor dialogs dockable and docked by default.
			// Updated the tutorials to describe 2-layer functionality.
//"1.81";	// Bug fix: Hiding the top toolbar made mouse clicks map to the wrong grid locations.
			// Bug fix: Right clicking on the top toolbar allowed control and component editor dialogs to be displayed together.
			// Added shell script to uninstall on Linux and updated README.txt with info on uninstall procedure.
//"1.82";	// Bug fix:  Merge should not update the filename used for saving.
//"1.83";	// Bug fix:  Pin dialog should be hidden in component editor mode.
			// Bug fix:  Vias were only drawn with correct size in PCB mode.
			// Improvement:  Zoom with repect to mouse position rather then centre of view.
			// Improvement:  When adding to part templates from current circuit, only consider currently selected parts.
			// Improvement:  Reduce minimum allowed track width from 30 mil to 12 mil.
			// New feature:  Allow use vias to be enabled/disabled under Layers menu.
			// New feature:  Added new "Thin" track style for more traditional look and smaller Gerber files.
  "1.84";	// Bug fix:  Repainting pins on layer 2 did not update grid connections properly, and could crash when the part is moved.

