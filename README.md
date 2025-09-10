# Week-4-



import React from "react";
import { motion } from "framer-motion";

export default function Diagram() {
  return (
    <div className="min-h-screen w-full bg-white text-gray-900 p-6">
      <div className="max-w-6xl mx-auto space-y-6">
        <header className="flex items-center justify-between">
          <h1 className="text-2xl font-bold">Board A / Board B – CAN ↔ UDP Bridge Flow Diagram</h1>
          <span className="text-sm text-gray-600">BRIDGE_UDP_PORT = 20000 · CAN = 500 kbit/s</span>
        </header>

        {/* Legend */}
        <div className="grid md:grid-cols-3 gap-4">
          <div className="p-4 rounded-2xl shadow bg-gray-50">
            <h2 className="font-semibold mb-2">Network Parameters</h2>
            <ul className="text-sm leading-6">
              <li><b>Board A</b>: 192.168.10.10 · MAC …:10</li>
              <li><b>Board B</b>: 192.168.10.20 · MAC …:20</li>
              <li>Netmask: 255.255.255.0 · GW: 192.168.10.1</li>
              <li>UDP Port: 20000 (unicast A → B)</li>
            </ul>
          </div>
          <div className="p-4 rounded-2xl shadow bg-gray-50">
            <h2 className="font-semibold mb-2">Hardware & Lines</h2>
            <ul className="text-sm leading-6">
              <li>120 Ω
