let component = ReasonReact.statelessComponent("index");

let loader = _context => Js.Promise.resolve();

let make = _data => {
  ...component,
  render: _self =>
    <div> {ReasonReact.string("Welcome to ReasonNext!")} </div>,
};

let default = ReasonNext.createPage(~loader, ~component, make);