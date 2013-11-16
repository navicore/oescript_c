#!/usr/bin/ruby
require 'test/unit'
require 'oescriptimpl'

class TestRubyMod < Test::Unit::TestCase

  def test_runme
  	signals = Oescriptimpl::OesSigHandler.new("mysigfile.pid", "oesrb", "info")
	assert_not_nil(signals)

	dispatcher = Oescriptimpl::OesDispatcher.new(1, "mydispatcher")
	assert_not_nil(dispatcher)
	dispatcher.start()

	tdispatcher = Oescriptimpl::OesThreadDispatcher.new(4)
	assert_not_nil(tdispatcher)
	tdispatcher.start()

	store = Oescriptimpl::OesStore.new(3, "data", 1)
	assert_not_nil(store)

	kernel = Oescriptimpl::OesKernel.new(dispatcher, tdispatcher, store,
    nil, nil, nil, nil)
	assert_not_nil(kernel)

	#start socket server
	net = Oescriptimpl::OesNet.new(dispatcher, false, nil, nil)
	assert_not_nil(net)
	json = Oescriptimpl::OesServer.new(dispatcher, "oejson://0.0.0.0:7777",net, store)
	assert_not_nil(json)
    json.addKernel("myspace", kernel)

  end
end

