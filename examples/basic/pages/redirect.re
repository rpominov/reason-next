let component = ReasonReact.statelessComponent("redirect");

let loader = context => {
  ReasonNext.redirect(~context, "/");
  Js.Promise.resolve();
};

let make = _data => component;

let default = ReasonNext.createPage(~loader, ~component, make);