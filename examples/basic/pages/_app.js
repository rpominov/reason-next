import App, {Container} from "next/app"
import * as React from "react"

export default class MyApp extends App {
  render() {
    const {Component, pageProps, router} = this.props
    return (
      <Container>
        <Component {...pageProps} router={router} />
      </Container>
    )
  }
}
