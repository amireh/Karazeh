#!/usr/bin/env ruby

ROOT = File.dirname(__FILE__)

require 'sinatra'

configure do
  set :port, 9333
  set :public_folder, "#{ROOT}/../fixture"
  set :public_folder, "/home/kandie/Workspace/Projects/ktest"
end

get '/' do
  "Karazeh test patch server"
end
