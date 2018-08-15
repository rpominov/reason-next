module LoadingContext: {
  module Req: {
    type t;
    [@bs.get] external headers: t => Js.Dict.t(string) = "";
    let isSecure: t => bool;
  };

  module Res: {
    type t;
    [@bs.get] external finished: t => bool = "";
    [@bs.get] external headersSent: t => bool = "";
    [@bs.get] external statusCode: t => int = "";
    [@bs.set] external statusCodeSet: (t, int) => unit = "statusCode";
    type headerValue =
      | None
      | One(string)
      | Many(array(string));
    let getHeader: (t, string) => headerValue;
    let setHeader: (t, string, headerValue) => unit;
    [@bs.send] external end_: t => unit = "end";
  };

  module BrowserRes: {type t;};

  module Err: {type t;};

  type t;
  [@bs.get] external pathname: t => string = "";
  [@bs.get] external query: t => Js.Dict.t(string) = "";
  [@bs.get] external asPath: t => string = "";
  [@bs.get] external req: t => Js.nullable(Req.t) = "";
  [@bs.get] external res: t => Js.nullable(Res.t) = "";
  [@bs.get] external jsonPageRes: t => Js.nullable(BrowserRes.t) = "";
  [@bs.get] external err: t => Js.nullable(Err.t) = "";
  let response: option(t) => option(Res.t);
  let request: option(t) => option(Req.t);
};

module Link: {
  let make:
    (
      ~href: 'a=?,
      ~_as: 'b=?,
      ~prefetch: bool=?,
      ~replace: bool=?,
      ~shallow: bool=?,
      ~passHref: bool=?,
      array(ReasonReact.reactElement)
    ) =>
    ReasonReact.component(
      ReasonReact.stateless,
      ReasonReact.noRetainedProps,
      ReasonReact.actionless,
    );
};

module Head: {
  let make:
    'a =>
    ReasonReact.component(
      ReasonReact.stateless,
      ReasonReact.noRetainedProps,
      ReasonReact.actionless,
    );
};

module Error: {
  let make:
    (~statusCode: int, 'a) =>
    ReasonReact.component(
      ReasonReact.stateless,
      ReasonReact.noRetainedProps,
      ReasonReact.actionless,
    );
};

module Router: {
  type t;
  [@bs.get] external asPath: t => string = "";
  [@bs.send] external replace: (t, string) => unit = "";
  let router: t;
  let onRouteChangeStart: option(unit => unit) => unit;
  let onRouteChangeComplete: option(unit => unit) => unit;
  let onRouteChangeError: option(unit => unit) => unit;
};

let createPage:
  (
    ~loader: LoadingContext.t => Js.Promise.t('a),
    ~component: ReasonReact.componentSpec(
                  'state,
                  'initialState,
                  'retainedProps,
                  'initialRetainedProps,
                  'action,
                ),
    'a => ReasonReact.component('state, 'retainedProps, 'action)
  ) =>
  ReasonReact.reactClass;

module Config: {
  type t;
  [@bs.get] external publicRuntimeConfig: t => Js.Dict.t(string) = "";
  let getConfig: unit => t;
};

let writeCookie:
  (~context: LoadingContext.t=?, ~maxAge: int=?, ~name: string, string) => unit;

let readCookie: (~context: LoadingContext.t=?, unit) => Js.Dict.t(string);

let redirect:
  (~context: LoadingContext.t=?, ~permanent: bool=?, string) => unit;

let setStatusCode: (~context: LoadingContext.t=?, int) => unit;

let getBaseUrl: (~context: LoadingContext.t=?, unit) => string;