# "Circus" task

The program is about coordinating a fast food restaurant, creating and completing orders from different clients that order different products in one order.

In our fast food restaurant there are machines that create products, each machines generate different product.

To our restaurant clients come during the day and they place orders. Every order has a unique number and pager to later collect the order.

Client when collecting the order, returns pager to the restaurant. If the client doesn't collect the order in time, then it cannot be collected later.

Every worker of the restaurant has to have few lists of orders that he was realizing during the day:
- list of collected orders - realized orders
- list of abondoned orders - orders not collected by clients
- list of failed orders - orders that for example, machine was broken and order couldn't be fullfilled
- list of failed products - products for which machines were broken and products were not able to be created

CoasterPager has methods that make client wait for the order, inform him that it's ready or to return the id of the order.

The System contains methods that: creates it, shutdowns it, return menu with products that can be created, return pending orders, makes client create a pending order to be created (if possible), makes client collect completed order from the worker.

The System is intended to enable parallel execution of 'numberOfWorkers' orders by representing employees in separate threads. The employee simultaneously waits for all the products he needs to order.

Files other than `system.cpp` were provided by the author of the task.
