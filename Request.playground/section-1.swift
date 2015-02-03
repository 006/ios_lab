/// The request that
///
/// In a real application this might be a bit more involved.
struct HTTPRequest {
  /// The kind of HTTP request.
  enum Method {
    case GET
    case POST
  }

  var path: String
  var method: Method
}

/// A handler for the HTTP server, handling requests with a certain kind
/// of payload data.
///
/// A handler can also choose whether or not to handle particular data based
/// on the request and the data's value.
protocol HTTPHandlerType {
  typealias Data

  /// :returns: true if the request was handled; false otherwise
  func handle(request: HTTPRequest, data: Data) -> Bool
}

/// An HTTP server that supports custom handlers dispatched by payload type.
class HTTPServer {

  /// Add a new handler to the server.
  ///
  /// Handlers are invoked in the order they are added.
  func addHandler<T: HTTPHandlerType>(handler: T) {
    handlers.append { (request: HTTPRequest, args: Any) -> Bool in
      if let typedArgs = args as? T.Data {
        return handler.handle(request, data: typedArgs)
      }
      return false
    }
  }

  private var handlers: [(HTTPRequest, Any) -> Bool] = []

  /// Handles an incoming request by trying each added handler in turn.
  /// 
  /// A Handler is only invoked if the argument data matches the handler's
  /// Data type.
  ///
  /// :returns: true if any handler was able to handle the request
  func dispatch(req: HTTPRequest, args: Any) -> Bool {
    for handler in handlers {
      if handler(req, args) {
        return true
      }
    }
    return false
  }

}

/// An example handler.
class MyHandler : HTTPHandlerType {
  func handle(request: HTTPRequest, data: Int) -> Bool {
    return data > 5
  }
}

let server = HTTPServer()
server.addHandler(MyHandler())
server.dispatch(HTTPRequest(path: "/update", method: .POST), args: "x")
server.dispatch(HTTPRequest(path: "/update", method: .POST), args: 5)
server.dispatch(HTTPRequest(path: "/update", method: .POST), args: 10)
