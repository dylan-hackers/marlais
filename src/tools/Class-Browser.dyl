module:		dylan-user
description:	A simple graphical class browser.

// need a require form to prevent multiple loads.
load("Windows.dyl");
load("Menus.dyl");

/*
define module Class-Browser
	use dylan-user, export: all;
	use Windows, export: all;
	use Menus, export: all;
end module Class-Browser;
*/

define method class-browser-updater (window :: <window>)
	let pen-position = vector (0, 0);

	local method move-to (x, y)
		pen-position[0] := x;
		pen-position[1] := y;
		window-move-to(window, x, y);
	end method move-to;
	
	local method move (dx, dy)
		pen-position[0] := pen-position[0] + dx;
		pen-position[1] := pen-position[1] + dy;
		window-move-to(window, pen-position[0], pen-position[1]);
	end method move;
	
	local method draw-all-classes() => ();
		move-to(10, 10);
		draw-subclasses(<object>, 0);
	end method print-all-classes;

	define method draw-subclasses(aClass :: <class>, level :: <integer>) => ();
		let indent = level * 20;
		if (indent > 0)
			move (indent, 0);
		end if;

		window-draw-string (window, as (<string>, debug-name(aClass)));
		
		move (-indent, 7);

		let subclasses = direct-subclasses(aClass);
		if (subclasses)
			for (subclass in subclasses)
				draw-subclasses(subclass, level + 1);
			end for;
		end if;
		values()
	end method print-subclasses;
	
	draw-all-classes();
end method class-browser-updater;

define method make-class-browser () => (window :: <window>);
	// create a new window. windows are initially invisible by default.
	let window = make(<window>,
					title: "Classes",
					bounds: #[50, 10, 450, 310],
					updater: class-browser-updater);
	// set up some attributes. a 3x3 pen.
	window.pen-size := #[3, 3];
	// use Geneva-9.
	window.text-style := make(<text-style>, font: "Geneva", size: 7);
	// make the window visible.
	window.visible := #t;
	window
end method make-class-browser;

// create a menu that controls windows.

define method browser-menu-behavior (menu :: <menu>, item :: <string>)
	if (item = "Create")
		make-class-browser();
	else
		let window = front-window();
		if (window)
			if (item = "Destroy")
				window-dispose(window);
			else
				window.visible := (item = "Show");
			end if;
		end if;
	end if;
end method;

define method browser-menu-adjuster (menu :: <menu>)
	let items = #("Destroy", "Show", "Hide");
	let state = (front-window() ~= #f);
	menu-set-item-states(menu, items, list(state, state, state));
end method browser-menu-adjuster;

define method make-browser-menu ()
	let menu = make(<menu>, title: "Browser", id: 1000,
					items: #("Create", "Destroy", "-", "Show", "Hide"),
					behavior: browser-menu-behavior,
					adjuster: browser-menu-adjuster);
	menu-enable-item(menu, "Create");
	browser-menu-adjuster (menu);
	menu-insert(menu);
	menu-draw();
	menu;
end method;

define variable *browser-menu* = make-browser-menu ();
