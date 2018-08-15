module LoadingContext = {
  module Req = {
    module Connection = {
      type secure;
      type t;
      [@bs.get] external secure: t => Js.Nullable.t(secure) = "";
    };

    type t;

    [@bs.get] external headers: t => Js.Dict.t(string) = "";
    [@bs.get] external connection: t => Connection.t = "";

    let isSecure = req =>
      switch (req->connection->Connection.secure->Js.Nullable.toOption) {
      | Some(_) => true
      | None => false
      };
  };

  module Res = {
    type t;

    [@bs.get] external finished: t => bool = "";
    [@bs.get] external headersSent: t => bool = "";
    [@bs.get] external statusCode: t => int = "";
    [@bs.set] external statusCodeSet: (t, int) => unit = "statusCode";

    type headerValue =
      | None
      | One(string)
      | Many(array(string));

    let getHeader_:
      (
        headerValue,
        string => headerValue,
        array(string) => headerValue,
        t,
        string
      ) =>
      headerValue = [%bs.raw
      {|
      function(none, one, many, res, name) {
        var result = res.getHeader(name);
        return result == null ? none : Array.isArray(result) ? many(result) : one(result);
      }
    |}
    ];
    [@bs.send] external _setHeader: (t, string, string) => unit = "setHeader";
    [@bs.send]
    external _setHeaderMany: (t, string, array(string)) => unit = "setHeader";
    [@bs.send] external _removeHeader: (t, string) => unit = "removeHeader";

    let getHeader = getHeader_(None, str => One(str), arr => Many(arr));
    let setHeader = (res, name, value) =>
      switch (value) {
      | None => _removeHeader(res, name)
      | One(str) => _setHeader(res, name, str)
      | Many(arr) => _setHeaderMany(res, name, arr)
      };
    [@bs.send] external end_: t => unit = "end";
  };

  module BrowserRes = {
    type t;
  };

  module Err = {
    type t;
  };

  type t;

  /* path section of URL */
  [@bs.get] external pathname: t => string = "";

  /* query string section of URL parsed as an object */
  [@bs.get] external query: t => Js.Dict.t(string) = "";

  /* String of the actual path (including the query) shows in the browser */
  [@bs.get] external asPath: t => string = "";

  /* HTTP request object (server only) */
  [@bs.get] external req: t => Js.nullable(Req.t) = "";

  /* HTTP response object (server only) */
  [@bs.get] external res: t => Js.nullable(Res.t) = "";

  /* Fetch Response object (client only) */
  [@bs.get] external jsonPageRes: t => Js.nullable(BrowserRes.t) = "";

  /* Error object if any error is encountered during the rendering */
  [@bs.get] external err: t => Js.nullable(Err.t) = "";

  let response = context =>
    switch (context) {
    | Some(ctx) => ctx->res->Js.Nullable.toOption
    | None => None
    };

  let request = context =>
    switch (context) {
    | Some(ctx) => ctx->req->Js.Nullable.toOption
    | None => None
    };
};

module Link = {
  [@bs.module "next/link"] external link: ReasonReact.reactClass = "default";
  let make =
      (
        ~href=?,
        ~_as=?,
        ~prefetch: option(bool)=?,
        ~replace: option(bool)=?,
        ~shallow: option(bool)=?,
        ~passHref: option(bool)=?,
        children: array(ReasonReact.reactElement),
      ) =>
    ReasonReact.wrapJsForReason(
      ~reactClass=link,
      ~props=
        Js.Undefined.{
          "href": fromOption(href),
          "as": fromOption(_as),
          "prefetch": fromOption(prefetch),
          "replace": fromOption(replace),
          "shallow": fromOption(shallow),
          "passHref": fromOption(passHref),
        },
      children,
    );
};

module Head = {
  [@bs.module "next/head"] external head: ReasonReact.reactClass = "default";
  let make = children =>
    ReasonReact.wrapJsForReason(
      ~reactClass=head,
      ~props=Js.Obj.empty(),
      children,
    );
};

module Error = {
  [@bs.module "next/error"] external error: ReasonReact.reactClass = "default";
  let make = (~statusCode: int, children) =>
    ReasonReact.wrapJsForReason(
      ~reactClass=error,
      ~props={"statusCode": statusCode},
      children,
    );
};

module Router = {
  type t;

  [@bs.get] external asPath: t => string = "";
  [@bs.send] external replace: (t, string) => unit = "";

  [@bs.set]
  external _changeStart: (t, Js.Nullable.t(unit => unit)) => unit =
    "onRouteChangeStart";
  [@bs.set]
  external _changeComplete: (t, Js.Nullable.t(unit => unit)) => unit =
    "onRouteChangeComplete";
  [@bs.set]
  external _changeError: (t, Js.Nullable.t(unit => unit)) => unit =
    "onRouteChangeError";

  [@bs.module "next/router"] external router: t = "default";

  let onRouteChangeStart = f =>
    _changeStart(router, f->Js.Nullable.fromOption);

  let onRouteChangeComplete = f =>
    _changeComplete(router, f->Js.Nullable.fromOption);

  let onRouteChangeError = f =>
    _changeError(router, f->Js.Nullable.fromOption);
};

module Page = {
  let create =
      (
        ~loader: LoadingContext.t => Js.Promise.t('a),
        ~component:
           ReasonReact.componentSpec(
             'state,
             'initialState,
             'retainedProps,
             'initialRetainedProps,
             'action,
           ),
        converter:
          'a => ReasonReact.component('state, 'retainedProps, 'action),
      ) => {
    let reactClass =
      ReasonReact.wrapReasonForJs(~component, props =>
        props##data |> PreserveTags.decode |> converter
      );

    let loader' = ctx =>
      Js.Promise.(
        loader(ctx)
        |> then_(x => {"data": PreserveTags.encode(x)} |> resolve)
      );

    Obj.magic(reactClass)##getInitialProps #= loader';
    reactClass;
  };
};

module Config = {
  type t;
  [@bs.get] external publicRuntimeConfig: t => Js.Dict.t(string) = "";
  [@bs.module "next/config"] external getConfig: unit => t = "default";
};

module Cookie = {
  [@bs.deriving abstract]
  type options = {
    [@bs.optional]
    maxAge: int,
  };

  [@bs.module "cookie"]
  external serialize: (string, string, options) => string = "";

  [@bs.module "cookie"] external parse: string => Js.Dict.t(string) = "";

  [@bs.get] external browserGet: Dom.document => string = "cookie";
  [@bs.set] external browserSet: (Dom.document, string) => unit = "cookie";
};

let writeCookie =
    (~context=?, ~maxAge=60 * 60 * 24 * 356, ~name: string, value: string) => {
  let cookieString =
    Cookie.serialize(name, value, Cookie.options(~maxAge, ()));

  switch (context->LoadingContext.response) {
  | Some(res) =>
    LoadingContext.Res.setHeader(
      res,
      "Set-Cookie",
      switch (LoadingContext.Res.getHeader(res, "Set-Cookie")) {
      | None => One(cookieString)
      | One(str) => Many([|str, cookieString|])
      | Many(arr) => Many(Js.Array.concat(arr, [|cookieString|]))
      },
    )
  | None => Webapi.Dom.document->Cookie.browserSet(cookieString)
  };
};

let createPage = Page.create;

let readCookie =
    (~context: option(LoadingContext.t)=?, ()): Js.Dict.t(string) =>
  Cookie.parse(
    switch (context->LoadingContext.request) {
    | Some(req) =>
      req->LoadingContext.Req.headers->Js.Dict.get("cookie")
      |> Js.Option.getWithDefault("")
    | None => Webapi.Dom.document->Cookie.browserGet
    },
  );

let redirect =
    (~context: option(LoadingContext.t)=?, ~permanent=false, target: string) =>
  switch (context->LoadingContext.response) {
  | Some(res) =>
    LoadingContext.Res.setHeader(res, "Location", One(target));
    LoadingContext.Res.statusCodeSet(res, permanent ? 301 : 303);
    LoadingContext.Res.end_(res);
  | None => Router.replace(Router.router, target)
  };

let setStatusCode = (~context: option(LoadingContext.t)=?, status: int) =>
  switch (context->LoadingContext.response) {
  | Some(res) => LoadingContext.Res.statusCodeSet(res, status)
  | None => ()
  };

let getBaseUrl = (~context: option(LoadingContext.t)=?, ()) =>
  switch (context->LoadingContext.request) {
  | Some(req) =>
    LoadingContext.Req.(
      (isSecure(req) ? "https" : "http")
      ++ "://"
      ++ req->headers->Js.Dict.unsafeGet("host")
    )
  | None =>
    Webapi.Dom.(location->Location.protocol ++ "//" ++ location->Location.host)
  };