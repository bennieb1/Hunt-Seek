// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;
using System.Collections.Generic;

public class GraphicsProgFPS_2026Target : TargetRules
{
	public GraphicsProgFPS_2026Target(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V6;

		ExtraModuleNames.AddRange( new string[] { "GraphicsProgFPS_2026" } );
	}
}
