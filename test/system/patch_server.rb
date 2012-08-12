#!/usr/bin/env ruby

ROOT = File.dirname(__FILE__)

require 'sinatra'

configure do
  set :public_folder, "#{ROOT}/../fixture"
end

get '/' do
  "Karazeh test patch server"
end
